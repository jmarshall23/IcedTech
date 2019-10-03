// main.cpp
//

#define RENDER_LIGHT_TOOL

#include "renderlight.h"
#include <sdl.h>
#include <SDL_syswm.h> 
#include "d3d12/D3D12_local.h"
#include "math/vectormath.h"
#include "d3d12/D3D12_RayOutputCopy.h"
#include "../../tools/compilers/megalight/MegaLight_file.h"

SDL_Window *window;
SDL_Surface *sdl_surface;
const UINT FrameCount = 2;

#define RUN_SILIENT

struct DirectXRaytraceBSP
{
	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indexes;
	std::vector<MegaLight_t> lights;
	DXRMesh dxrmesh;
};

DirectXRaytraceBSP raytraceBSP;

int lightmapsize = -1;

int LoadFile(const char *fileName, void **buffer)
{
	FILE *file = fopen(fileName, "rb");
	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	*buffer = malloc(length);
	fread(*buffer, 1, length, file);
	fclose(file);
	return length;
}

void LoadRenderLightFile(const char *mapName)
{
	MegaLightFileHeader_t *header;
	LoadFile(mapName, (void **)&header);

	MegaLightVertex_t *vertex = (MegaLightVertex_t *)(((byte *)header) + sizeof(MegaLightFileHeader_t));
	unsigned int *indexes = (unsigned int *)(((byte *)vertex) + (sizeof(MegaLightVertex_t) * header->numVertexes));
	MegaLight_t *lights = (MegaLight_t *)(((byte *)indexes) + (sizeof(unsigned int) * header->numIndexes));

	for (int i = 0; i < header->numVertexes; i++)
	{
		Vertex v;
		v.position.x = vertex[i].xyz[0];
		v.position.y = vertex[i].xyz[2];
		v.position.z = vertex[i].xyz[1];

		v.texcoord.x = vertex[i].st[0];
		v.texcoord.y = vertex[i].st[1];
		if (vertex[i].isMegaTextureGeometry)
		{
			v.texcoord.z = 1.0;
		}
		else
		{
			v.texcoord.z = 0.0;
		}

		v.normal.x = vertex[i].normal[0];
		v.normal.y = vertex[i].normal[1];
		v.normal.z = vertex[i].normal[2];
		raytraceBSP.vertexes.push_back(v);
	}

	for (int i = 0; i < header->numIndexes; i++)
	{
		raytraceBSP.indexes.push_back(indexes[i]);
	}

	for (int i = 0; i < header->numLights; i++)
	{
		MegaLight_t light = lights[i];
		light.origin[0] = lights[i].origin[0];
		light.origin[1] = -lights[i].origin[1];
		light.origin[2] = lights[i].origin[2];

		light.color[0] = lights[i].color[0];
		light.color[1] = lights[i].color[1];
		light.color[2] = lights[i].color[2];
		raytraceBSP.lights.push_back(light);
	}

	free(header);


	raytraceBSP.dxrmesh = DXR_BuildGeometry(raytraceBSP.vertexes, raytraceBSP.indexes);
}

int rotationYAW[6] = { 0, 90, 180, 270, 0, 0 };
int rotationPITCH[6] = { 0, 0, 0, 0, 90, 270 };


void SetCameraInfo(MegaLight_t *light, int frameIndex, int viewport_width, int viewport_height)
{
	float m_aspectRatio = ((float)viewport_width) / ((float)viewport_height);
	float cameraX = light->origin[0];
	float cameraY = light->origin[2];
	float cameraZ = -light->origin[1];

	//float4x4 projectionMatrix;
	//polymer_createprojectionmatrix(pr_fov, projectionMatrix, xdim, ydim);
	for (int i = 0; i < 6; i++)
	{
		float4x4 viewMatrix = float4x4Identity();
		float4 position = float4(cameraX, cameraY, cameraZ, 1.0f);

		float pitch = rotationPITCH[i];
		float yaw = rotationYAW[i];

		_math_matrix_rotate(viewMatrix, 0, 0.0f, 0.0f, -1.0f);
		_math_matrix_rotate(viewMatrix, pitch, 1.0f, 0.0f, 0.0f);
		_math_matrix_rotate(viewMatrix, yaw, 0.0f, 1.0f, 0.0f);

		viewMatrix.transpose();
		float *viewMatrixPtr = &viewMatrix.r0.x;

		XMFLOAT4 cameraPosition = XMFLOAT4(position.x, position.y, position.z, 1.0f);
		XMVECTOR cameraPositionVector = XMLoadFloat4(&cameraPosition);

		XMFLOAT4 up = XMFLOAT4(viewMatrixPtr[4], viewMatrixPtr[5], viewMatrixPtr[6], 1.0f);
		XMVECTOR upVector = XMLoadFloat4(&up);

		XMFLOAT4 lookat = XMFLOAT4(position.x + -viewMatrixPtr[8], position.y + -viewMatrixPtr[9], position.z + -viewMatrixPtr[10], 1.0f);
		XMVECTOR lookatvector = XMLoadFloat4(&lookat);

		XMMATRIX view = XMMatrixLookAtLH(cameraPositionVector, lookatvector, upVector);
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), m_aspectRatio, 1.0f, 10025.0f);

		XMMATRIX viewProj = view * proj;

		float intensity = 1000000.0f;

		d3d12CoreState->m_sceneCB[frameIndex].cameraPosition = { position.x, position.y, position.z, position.w };
		d3d12CoreState->m_sceneCB[frameIndex].lightColor = { light->color[0], light->color[1], light->color[2], 1.0f };
		d3d12CoreState->m_sceneCB[frameIndex].lightInfo = { intensity, 0.0f, (float)lightmapsize, 0.0f };
		d3d12CoreState->m_sceneCB[frameIndex].projectionToWorld[i] = XMMatrixInverse(nullptr, viewProj);
	}
}


extern "C" int renderlight(int argc, char **argv)
{
	int currentLight = 0;

	LogPrint("RenderLight v0.01 Build(%s,%s) by Justin Marshall\n", __DATE__, __TIME__);

	if (argc < 2)
	{
		LogPrint("Invalid usage!\n");
		return -1;
	}

	lightmapsize = atoi(argv[3]);

	// Create tool window.
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		argv[1],                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		lightmapsize,                               // width, in pixels
		lightmapsize,                               // height, in pixels
#ifdef RUN_SILIENT
		SDL_WINDOW_HIDDEN                  // flags - see below
#else
		SDL_WINDOW_SHOWN
#endif
	);

	sdl_surface = SDL_GetWindowSurface(window);

	SDL_SetWindowFullscreen(window, 0);

	//get the native whnd 
	struct SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND window_hwnd = wmInfo.info.win.window;

	LogPrint("Initializing Direct3D 12...\n");

	m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, FrameCount, D3D_FEATURE_LEVEL_11_0, DX::DeviceResources::c_RequireTearingSupport, 0);
	m_deviceResources->SetWindow(window_hwnd, lightmapsize, lightmapsize);
	m_deviceResources->InitializeDXGIAdapter();

	std::string rtx_devicename = DX::wstr_to_astr(m_deviceResources->GetAdapterDescription());
	LogPrint("Using Device %s\n", rtx_devicename.c_str());

	LogPrint("Enabling DirectX Raytracing(DXR)...\n");

	// TODO: Add DXR fallback.
	if (!EnableRaytracing(m_deviceResources->GetAdapter()))
	{
		LogError("Unable to enable experimental raytracing featureset, DXR fallback is not implemented yet. Init Failed");
		return -1;
	}

	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	d3d12CoreState = new D3D12CoreState(lightmapsize, lightmapsize);

	R_InitRayOutputCopy(lightmapsize);

	// Load in our bsp file.
	LogPrint("Loading RenderLight file: %s\n", argv[1]);
	LoadRenderLightFile(argv[1]);

	LogPrint("Setup Complete\n");
	while (true)
	{
		SDL_Event event;
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
		{
			break;
		}

		UINT frameIndex = -1;

		m_deviceResources->Prepare();
		frameIndex = m_deviceResources->GetCurrentFrameIndex();

		auto commandList = m_deviceResources->GetCommandList();
		auto renderTarget = m_deviceResources->GetRenderTarget();

		// Clear the render target and depth stencil.
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		commandList->ClearRenderTargetView(m_deviceResources->GetRenderTargetView(), clearColor, 0, nullptr);

		SetCameraInfo(&raytraceBSP.lights[(int)currentLight], frameIndex, lightmapsize, lightmapsize);

		d3d12CoreState->m_sceneCB[frameIndex].lightId = ((int)currentLight % 254) + 1;
#ifdef RUN_SILIENT
		LogPrint("Light Bake %d/%d\n", currentLight, raytraceBSP.lights.size());
#endif
		DXR_DoRaytracing(lightmapsize, lightmapsize, &raytraceBSP.dxrmesh, d3d12CoreState->m_sceneCB[frameIndex]);

		R_ComputeCopyResultToFinal(commandList);

		DXR_CopyToBackBuffer();

		m_deviceResources->Present(D3D12_RESOURCE_STATE_PRESENT);
		m_deviceResources->WaitForGpu();


		currentLight++;
		if (currentLight >= raytraceBSP.lights.size())
		{
#ifdef RUN_SILIENT
			R_SaveRaytracingOutput(argv[2]);
			break;
#else
			currentLight = 0;
#endif

		}
	}

	SDL_Quit();

	return 0;
}

