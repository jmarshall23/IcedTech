// D3D12_DXR_Scene.cpp
//

#include "../renderlight.h"
#include "d3d12_local.h"

#if 0
DXRScene *DXR_CreateSceneFromMeshes(std::vector<DXRMesh> &meshes) 
{
	DXRScene *scene = new DXRScene();
	auto device = m_deviceResources->GetD3DDevice();
	auto commandList = m_deviceResources->GetCommandList();
	auto commandQueue = m_deviceResources->GetCommandQueue();
	auto commandAllocator = m_deviceResources->GetCommandAllocator();

	scene->meshes = meshes;

	// Reset the command list for the acceleration structure construction.
	commandList->Reset(commandAllocator, nullptr);

	D3D12_RESOURCE_STATES initialResourceState;
	if (raytracingAPI == RaytracingAPI::FallbackLayer)
	{
		initialResourceState = d3d12CoreState->m_fallbackDevice->GetAccelerationStructureResourceState();
	}
	else // DirectX Raytracing
	{
		initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	}

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	D3D12_GET_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO_DESC prebuildInfoDesc = {};
	prebuildInfoDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	prebuildInfoDesc.Flags = buildFlags;
	prebuildInfoDesc.NumDescs = 1;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
	prebuildInfoDesc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	prebuildInfoDesc.pGeometryDescs = nullptr;
	if (raytracingAPI == RaytracingAPI::FallbackLayer)
	{
		d3d12CoreState->m_fallbackDevice->GetRaytracingAccelerationStructurePrebuildInfo(&prebuildInfoDesc, &topLevelPrebuildInfo);
	}
	else // DirectX Raytracing
	{
		d3d12CoreState->m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&prebuildInfoDesc, &topLevelPrebuildInfo);
	}
	ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	ComPtr<ID3D12Resource> scratchResource;
	AllocateUAVBuffer(device, topLevelPrebuildInfo.ScratchDataSizeInBytes, &scratchResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");
	AllocateUAVBuffer(device, topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &scene->m_topLevelAccelerationStructure, initialResourceState, L"TopLevelAccelerationStructure");

	// The primary point of divergence between the DXR API and the compute-based Fallback layer is the handling of GPU pointers. 
	// DXR fundamentally requires that GPUs be able to dynamically read from arbitrary addresses in GPU memory. 
	// The existing Direct Compute API today is more rigid than DXR and requires apps to explicitly inform the GPU what blocks of memory it will access with SRVs/UAVs.
	// In order to handle the requirements of DXR, the Fallback Layer uses the concept of Emulated GPU pointers, 
	// which requires apps to create views around all memory they will access for raytracing, 
	// but retains the DXR-like flexibility of only needing to bind the top level acceleration structure at DispatchRays.
	//
	// The Fallback Layer interface uses WRAPPED_GPU_POINTER to encapsulate the underlying pointer
	// which will either be an emulated GPU pointer for the compute - based path or a GPU_VIRTUAL_ADDRESS for the DXR path.

	// Create an instance desc for the bottom-level acceleration structure.
	ComPtr<ID3D12Resource> instanceDescResource;
	std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescPool;

	for (int i = 0; i < meshes.size(); i++)
	{
		D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
		instanceDesc.Transform[0] = instanceDesc.Transform[5] = instanceDesc.Transform[10] = 1;
		instanceDesc.InstanceMask = 1;
		instanceDesc.AccelerationStructure = meshes[i].m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
		instanceDescPool.push_back(instanceDesc);
	}

	AllocateUploadBuffer(device, &instanceDescPool[0], sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceDescPool.size(), &instanceDescResource, L"InstanceDescs");

	// Top Level Acceleration Structure desc
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = { };
	{
		topLevelBuildDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		topLevelBuildDesc.Flags = buildFlags;
		topLevelBuildDesc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		topLevelBuildDesc.DestAccelerationStructureData = { scene->m_topLevelAccelerationStructure->GetGPUVirtualAddress(), topLevelPrebuildInfo.ResultDataMaxSizeInBytes };
		topLevelBuildDesc.NumDescs = meshes.size();
		topLevelBuildDesc.pGeometryDescs = nullptr;
		topLevelBuildDesc.InstanceDescs = instanceDescResource->GetGPUVirtualAddress(); // jmarshall: THIS IS OBVIOUSLY WRONG!!!
		topLevelBuildDesc.ScratchAccelerationStructureData = { scratchResource->GetGPUVirtualAddress(), scratchResource->GetDesc().Width };
	}

	d3d12CoreState->m_dxrCommandList.Get()->BuildRaytracingAccelerationStructure(&topLevelBuildDesc);

	// Kick off acceleration structure construction.
	m_deviceResources->ExecuteCommandList();

	// Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
	m_deviceResources->WaitForGpu();

	return scene;
}
#endif