#pragma once

#include "../shaders/RayTracingHlslCompat.h"


namespace GlobalRootSignatureParams {
	enum Value {
		OutputViewSlot = 0,
		AccelerationStructureSlot,
		SceneConstantSlot,
		VertexBuffersSlot,
		Count
	};
}

namespace LocalRootSignatureParams {
	enum Value {
		CubeConstantSlot = 0,
		Count
	};
}

union AlignedSceneConstantBuffer
{
	SceneConstantBuffer constants;
	uint8_t alignmentPadding[D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT * 2];
};

class DXRMesh {
public:
//	WRAPPED_GPU_POINTER m_fallbackTopLevelAccelerationStructurePointer;
	ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
	ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;

	D3DBuffer scene_vertexBuffer;
	D3DBuffer scene_indexBuffer;
};

struct DXRScene {
	std::vector<DXRMesh> meshes;
	ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;
};

class D3D12CoreState
{
public:
	D3D12CoreState(int width, int height);
	~D3D12CoreState();

	// We'll allocate space for several of these and they will need to be padded for alignment.
//	static_assert(sizeof(SceneConstantBuffer) < D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, "Checking the size here.");

	AlignedSceneConstantBuffer*  m_mappedConstantData;
	ComPtr<ID3D12Resource>       m_perFrameConstants;

	//int64_t				 m_constantDataSizePadded;

	// Raytracing Fallback Layer (FL) attributes
	ComPtr<ID3D12RaytracingFallbackDevice> m_fallbackDevice;
	ComPtr<ID3D12RaytracingFallbackCommandList> m_fallbackCommandList;
	ComPtr<ID3D12RaytracingFallbackStateObject> m_fallbackStateObject;

	// DirectX Raytracing (DXR) attributes
	ComPtr<ID3D12Device5> m_dxrDevice;
	ComPtr<ID3D12GraphicsCommandList4> m_dxrCommandList;
	ComPtr<ID3D12StateObject> m_dxrStateObject;
	bool m_isDxrSupported;

	// Root signatures
	ComPtr<ID3D12RootSignature> m_raytracingGlobalRootSignature;
	ComPtr<ID3D12RootSignature> m_raytracingLocalRootSignature;

	CubeConstantBuffer m_cubeCB;

	// Descriptors
	ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	UINT m_descriptorsAllocated;
	UINT m_descriptorSize;

	// Raytracing scene
	SceneConstantBuffer m_sceneCB[3];
//	CubeConstantBuffer m_cubeCB;

	// Acceleration structure
	ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
	ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;

	// Raytracing output
	ComPtr<ID3D12Resource> m_raytracingOutput;
	D3D12_GPU_DESCRIPTOR_HANDLE m_raytracingOutputResourceUAVGpuDescriptor;
	UINT m_raytracingOutputResourceUAVDescriptorHeapIndex;

	// Shader tables
	ComPtr<ID3D12Resource> m_missShaderTable;
	ComPtr<ID3D12Resource> m_hitGroupShaderTable;
	ComPtr<ID3D12Resource> m_rayGenShaderTable;

	UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse);
private:
	void CreateDeviceDependentResources();

private:
	void CreateConstantBuffers();
	void SerializeAndCreateRaytracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig);
	void CreateRootSignatures();
	void CreateRaytracingInterfaces();
	void CreateRaytracingPipelineStateObject();
	void CreateRaytracingOutputResource();
	void CreateDescriptorHeap();
	void BuildShaderTables();
	void CreateLocalRootSignatureSubobjects(CD3D12_STATE_OBJECT_DESC* raytracingPipeline);

	int m_width;
	int m_height;

	char *raytraceShaderBuffer;
	int raytraceShaderBufferLen;
};

extern D3D12CoreState *d3d12CoreState;