// D3D12_DXR_Render.cpp
//

#include "../renderlight.h"
#include "d3d12_local.h"

void DXR_DoRaytracing(int width, int height, DXRMesh *mesh, SceneConstantBuffer &m_sceneCB)
{
	auto commandList = m_deviceResources->GetCommandList();
	auto frameIndex = m_deviceResources->GetCurrentFrameIndex();

	commandList->SetComputeRootSignature(d3d12CoreState->m_raytracingGlobalRootSignature.Get());

	byte *constantBufferPtr = ((byte *)&d3d12CoreState->m_mappedConstantData[0]) + frameIndex * sizeof(AlignedSceneConstantBuffer);

	// Copy the updated scene constant buffer to GPU.
	memcpy(constantBufferPtr, &m_sceneCB, sizeof(m_sceneCB));
	auto cbGpuAddress = d3d12CoreState->m_perFrameConstants->GetGPUVirtualAddress() + frameIndex * sizeof(AlignedSceneConstantBuffer);
	commandList->SetComputeRootConstantBufferView(GlobalRootSignatureParams::SceneConstantSlot, cbGpuAddress);

	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
	commandList->SetDescriptorHeaps(1, d3d12CoreState->m_descriptorHeap.GetAddressOf());
	// Set index and successive vertex buffer decriptor tables
	commandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::VertexBuffersSlot, mesh->scene_indexBuffer.gpuDescriptorHandle);
	commandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, d3d12CoreState->m_raytracingOutputResourceUAVGpuDescriptor);
	commandList->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot, mesh->m_topLevelAccelerationStructure->GetGPUVirtualAddress());
	//DispatchRays(d3d12CoreState->m_dxrCommandList.Get(), d3d12CoreState->m_dxrStateObject.Get(), &dispatchDesc, width, height);

	// Since each shader table has only one shader record, the stride is same as the size.
	dispatchDesc.HitGroupTable.StartAddress = d3d12CoreState->m_hitGroupShaderTable->GetGPUVirtualAddress();
	dispatchDesc.HitGroupTable.SizeInBytes = d3d12CoreState->m_hitGroupShaderTable->GetDesc().Width;
	dispatchDesc.HitGroupTable.StrideInBytes = dispatchDesc.HitGroupTable.SizeInBytes;
	dispatchDesc.MissShaderTable.StartAddress = d3d12CoreState->m_missShaderTable->GetGPUVirtualAddress();
	dispatchDesc.MissShaderTable.SizeInBytes = d3d12CoreState->m_missShaderTable->GetDesc().Width;
	dispatchDesc.MissShaderTable.StrideInBytes = dispatchDesc.MissShaderTable.SizeInBytes;
	dispatchDesc.RayGenerationShaderRecord.StartAddress = d3d12CoreState->m_rayGenShaderTable->GetGPUVirtualAddress();
	dispatchDesc.RayGenerationShaderRecord.SizeInBytes = d3d12CoreState->m_rayGenShaderTable->GetDesc().Width;
	dispatchDesc.Width = width;
	dispatchDesc.Height = height;
	dispatchDesc.Depth = 1;
	d3d12CoreState->m_dxrCommandList.Get()->SetPipelineState1(d3d12CoreState->m_dxrStateObject.Get());
	d3d12CoreState->m_dxrCommandList.Get()->DispatchRays(&dispatchDesc);
}

void DXR_CopyToBackBuffer(void)
{
	auto commandList = m_deviceResources->GetCommandList();

	auto renderTarget = m_deviceResources->GetRenderTarget();
	D3D12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(d3d12CoreState->m_raytracingOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

	commandList->CopyResource(renderTarget, d3d12CoreState->m_raytracingOutput.Get());

	D3D12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(d3d12CoreState->m_raytracingOutput.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	commandList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
}