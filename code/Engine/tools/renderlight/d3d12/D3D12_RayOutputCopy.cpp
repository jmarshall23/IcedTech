// D3D12_RayOutputCopy.cpp
//

#include "../renderlight.h"
#include "d3d12_local.h"
#include "D3D12_ScreenGrab.h"

ComPtr<ID3D12PipelineState> m_rayOutputCopyState;
ComPtr<ID3D12RootSignature> m_rayOutputComputeRootSignature;

ComPtr<ID3D12Resource> m_finalraytracingOutput;
D3D12_GPU_DESCRIPTOR_HANDLE m_finalraytracingOutputResourceUAVGpuDescriptor;
UINT m_finalraytracingOutputResourceUAVDescriptorHeapIndex = UINT_MAX;

int global_ouputcopy_dimen = -1;

static bool hasSwitchedD3D12TextureState = false;

namespace GlobalRayOutputCopyParams {
	enum Value {
		RenderTargetSlot = 0,
		FinalRenderTargetSlot,
		Count
	};
}

void R_InitRayOutputCopy(int lightmapDimen)
{
	LogPrint("Init RayOutput Copy...\n");
	global_ouputcopy_dimen = lightmapDimen;

	byte *rayOutputCopyShader;
	int rayOutputCopyLen = LoadFile("shaderbin/RayOutputCopy.computebin", (void **)&rayOutputCopyShader);
	if (rayOutputCopyLen <= 0 || rayOutputCopyShader == nullptr)
	{
		LogError("Failed to load output copy shader!\n");
	}

	{
		CD3DX12_DESCRIPTOR_RANGE ranges[2];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);

		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRayOutputCopyParams::Count];
		rootParameters[GlobalRayOutputCopyParams::RenderTargetSlot].InitAsDescriptorTable(1, &ranges[0]);
		rootParameters[GlobalRayOutputCopyParams::FinalRenderTargetSlot].InitAsDescriptorTable(1, &ranges[1]);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		CD3DX12_ROOT_SIGNATURE_DESC computeRootSignatureDesc(_countof(rootParameters), rootParameters, 0, nullptr);
		ThrowIfFailed(D3D12SerializeRootSignature(&computeRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));

		ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rayOutputComputeRootSignature)));
		NAME_D3D12_OBJECT(m_rayOutputComputeRootSignature);
	}

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePsoDesc = {};
	computePsoDesc.pRootSignature = m_rayOutputComputeRootSignature.Get();
	computePsoDesc.CS = CD3DX12_SHADER_BYTECODE(rayOutputCopyShader, rayOutputCopyLen);

	ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateComputePipelineState(&computePsoDesc, IID_PPV_ARGS(&m_rayOutputCopyState)));
	NAME_D3D12_OBJECT(m_rayOutputCopyState);

	// Create our final copy shader.
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto backbufferFormat = m_deviceResources->GetBackBufferFormat();

		// Create the output resource. The dimensions and format should match the swap-chain.
		auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(backbufferFormat, lightmapDimen, lightmapDimen, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(device->CreateCommittedResource(
			&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_finalraytracingOutput)));
		NAME_D3D12_OBJECT(m_finalraytracingOutput);

		D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle;
		m_finalraytracingOutputResourceUAVDescriptorHeapIndex = d3d12CoreState->AllocateDescriptor(&uavDescriptorHandle, m_finalraytracingOutputResourceUAVDescriptorHeapIndex);
		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		device->CreateUnorderedAccessView(m_finalraytracingOutput.Get(), nullptr, &UAVDesc, uavDescriptorHandle);
		m_finalraytracingOutputResourceUAVGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(d3d12CoreState->m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_finalraytracingOutputResourceUAVDescriptorHeapIndex, d3d12CoreState->m_descriptorSize);
	}
}

void R_ComputeCopyResultToFinal(ID3D12GraphicsCommandList *commandList)
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3d12CoreState->m_raytracingOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PRESENT));
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3d12CoreState->m_raytracingOutput.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	if(hasSwitchedD3D12TextureState)
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_finalraytracingOutput.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	commandList->SetPipelineState(m_rayOutputCopyState.Get());
	commandList->SetComputeRootSignature(m_rayOutputComputeRootSignature.Get());

	commandList->SetDescriptorHeaps(1, d3d12CoreState->m_descriptorHeap.GetAddressOf());
	// Set index and successive vertex buffer decriptor tables
	commandList->SetComputeRootDescriptorTable(GlobalRayOutputCopyParams::RenderTargetSlot, d3d12CoreState->m_raytracingOutputResourceUAVGpuDescriptor);
	commandList->SetComputeRootDescriptorTable(GlobalRayOutputCopyParams::FinalRenderTargetSlot, m_finalraytracingOutputResourceUAVGpuDescriptor);
	
	commandList->Dispatch(global_ouputcopy_dimen, global_ouputcopy_dimen, 1);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_finalraytracingOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PRESENT));

	hasSwitchedD3D12TextureState = true;
}

void R_SaveRaytracingOutput(char *filename)
{
	// And were done!
	char lightmapfile[512];
	sprintf(lightmapfile, "%s.tga", filename);
	DirectX::SaveTGATextureToFile(m_deviceResources->GetCommandQueue(), m_finalraytracingOutput.Get(), lightmapfile, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT);
}