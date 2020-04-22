// engine_d3d12.h
//

#pragma once

#include "D3D12_wrl.h"

#include <dxgi1_6.h>
//#include <d3d12_1.h>
#include "../d3d12libs/d3dx12.h"

#include <DirectXMath.h>

enum class RaytracingAPI {
	NoRaytracing,
	FallbackLayer,
	DirectXRaytracing,
};

extern RaytracingAPI raytracingAPI;

#define _ATL_ALLOW_CHAR_UNSIGNED

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
using Microsoft::WRL::ComPtr;

class HrException : public std::runtime_error
{
	inline std::string HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_str);
	}
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
	HRESULT Error() const { return m_hr; }
private:
	const HRESULT m_hr;
};

#define SAFE_RELEASE(p) if (p) (p)->Release()

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw HrException(hr);
	}
}

inline void ThrowIfFailed(HRESULT hr, const wchar_t* msg)
{
	if (FAILED(hr))
	{
		OutputDebugStringW(msg);
		throw HrException(hr);
	}
}

inline void ThrowIfFalse(bool value)
{
	ThrowIfFailed(value ? S_OK : E_FAIL);
}

inline void ThrowIfFalse(bool value, const wchar_t* msg)
{
	ThrowIfFailed(value ? S_OK : E_FAIL, msg);
}


inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
	if (path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileNameW(nullptr, path, pathSize);
	if (size == 0 || size == pathSize)
	{
		// Method failed or path was truncated.
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash)
	{
		*(lastSlash + 1) = L'\0';
	}
}

inline HRESULT ReadDataFromFile(LPCWSTR filename, byte** data, UINT* size)
{
	using namespace Microsoft::WRL;

	CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
	extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
	extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
	extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
	extendedParams.lpSecurityAttributes = nullptr;
	extendedParams.hTemplateFile = nullptr;

	Wrappers::FileHandle file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
	if (file.Get() == INVALID_HANDLE_VALUE)
	{
		throw std::exception();
	}

	FILE_STANDARD_INFO fileInfo = {};
	if (!GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
	{
		throw std::exception();
	}

	if (fileInfo.EndOfFile.HighPart != 0)
	{
		throw std::exception();
	}

	*data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
	*size = fileInfo.EndOfFile.LowPart;

	if (!ReadFile(file.Get(), *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
	{
		throw std::exception();
	}

	return S_OK;
}

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
	pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
	WCHAR fullName[50];
	if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
	{
		pObject->SetName(fullName);
	}
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

inline UINT Align(UINT size, UINT alignment)
{
	return (size + (alignment - 1)) & ~(alignment - 1);
}

inline UINT CalculateConstantBufferByteSize(UINT byteSize)
{
	// Constant buffer size is required to be aligned.
	return Align(byteSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
}

#ifdef D3D_COMPILE_STANDARD_FILE_INCLUDE
inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	return byteCode;
}
#endif

// Resets all elements in a ComPtr array.
template<class T>
void ResetComPtrArray(T* comPtrArray)
{
	for (auto &i : *comPtrArray)
	{
		i.Reset();
	}
}


// Resets all elements in a unique_ptr array.
template<class T>
void ResetUniquePtrArray(T* uniquePtrArray)
{
	for (auto &i : *uniquePtrArray)
	{
		i.reset();
	}
}

class GpuUploadBuffer
{
public:
	ComPtr<ID3D12Resource> GetResource() { return m_resource; }

protected:
	ComPtr<ID3D12Resource> m_resource;

	GpuUploadBuffer() {}
	~GpuUploadBuffer()
	{
		if (m_resource.Get())
		{
			m_resource->Unmap(0, nullptr);
		}
	}

	void Allocate(ID3D12Device* device, UINT bufferSize, LPCWSTR resourceName = nullptr)
	{
		auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_resource)));
		m_resource->SetName(resourceName);
	}

	uint8_t* MapCpuWriteOnly()
	{
		uint8_t* mappedData;
		// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)));
		return mappedData;
	}
};

struct D3DBuffer
{
	ComPtr<ID3D12Resource> resource;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
};

// Helper class to create and update a constant buffer with proper constant buffer alignments.
// Usage: ToDo
//    ConstantBuffer<...> cb;
//    cb.Create(...);
//    cb.staging.var = ...; | cb->var = ... ; 
//    cb.CopyStagingToGPU(...);
template <class T>
class ConstantBuffer : public GpuUploadBuffer
{
	uint8_t* m_mappedConstantData;
	UINT m_alignedInstanceSize;
	UINT m_numInstances;

public:
	ConstantBuffer() : m_alignedInstanceSize(0), m_numInstances(0), m_mappedConstantData(nullptr) {}

	void Create(ID3D12Device* device, UINT numInstances = 1, LPCWSTR resourceName = nullptr)
	{
		m_numInstances = numInstances;
		UINT alignedSize = Align(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		UINT bufferSize = numInstances * alignedSize;
		Allocate(device, bufferSize, resourceName);
		m_mappedConstantData = MapCpuWriteOnly();
	}

	void CopyStagingToGpu(UINT instanceIndex = 0)
	{
		memcpy(m_mappedConstantData + instanceIndex * m_alignedInstanceSize, &staging, sizeof(T));
	}

	// Accessors
	T staging;
	T* operator->() { return &staging; }
	UINT NumInstances() { return m_numInstances; }
	D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress(UINT instanceIndex = 0)
	{
		return m_resource->GetGPUVirtualAddress() + instanceIndex * m_alignedInstanceSize;
	}
};

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include "DirectXRaytracingHelper.h"
#include "../d3d12libs/D3D12RaytracingFallback.h"
#include "../d3d12libs/D3D12RaytracingHelpers.hpp"
#include "D3D12_DeviceResources.h"
#include "D3D12_DXR_CoreState.h"

#include "d3d12_timer.h"

DXRMesh DXR_BuildGeometry(std::vector<Vertex> &vertexes, std::vector<unsigned int> &indexes);
DXRScene *DXR_CreateSceneFromMeshes(std::vector<DXRMesh> &meshes);
void DXR_DoRaytracing(int width, int height, DXRMesh *mesh, SceneConstantBuffer &m_sceneCB);
void DXR_CopyToBackBuffer(void);

extern std::unique_ptr<DX::DeviceResources> m_deviceResources;

//
// PipelineState
//
class D3D12PipelineState {
public:
	D3D12PipelineState(ComPtr<ID3D12PipelineState> m_pipelineState) {
		this->m_pipelineState = m_pipelineState;
	}
	static D3D12PipelineState *CreatePipelineState(const char *shaderName, ComPtr<ID3D12RootSignature> &m_rootSignature, bool isGUI, bool isDepthEnabled, bool isStencilEnabled, int numRenderTargets);
private:
	ComPtr<ID3D12PipelineState> m_pipelineState;
};

//
// D3D12_Gui_Render.cpp
//
void D3D12_InitGUI(void);

//
// D3D12GuiVertex
//
struct D3D12GuiVertex {
	float xyz[3];
	float texcoord[2];
};

void D3D12_DrawGUI(D3D12GuiVertex *vertexes, int tilenum);

//
// D3D12_Texture.cpp
//
enum D3D12ImageType {
	D3D12_IMAGETYPE_TEXTURE1D = 0,
	D3D12_IMAGETYPE_TEXTURE2D
};

class D3D12Texture {
public:
	ComPtr<ID3D12Resource> m_texture;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	UINT m_rtvDescriptorSize;
	int width;
	int height;
};

D3D12Texture *D3D12_CreateTexture(const wchar_t *name, D3D12ImageType imageType, int TextureWidth, int TextureHeight, int bpp, void *buffer);

__forceinline UINT D3D12_Align(UINT location, UINT align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)
{
	return (location + (align - 1)) & ~(align - 1);
}