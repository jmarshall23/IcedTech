// D3D12_PipelineState.cpp
//

#include "../renderlight.h"
#include "d3d12_local.h"

D3D12_INPUT_ELEMENT_DESC gui_shaderdesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

D3D12PipelineState *D3D12PipelineState::CreatePipelineState(const char *shaderName, ComPtr<ID3D12RootSignature> &m_rootSignature, bool isGUI, bool isDepthEnabled, bool isStencilEnabled, int numRenderTargets) {
	return nullptr;
}