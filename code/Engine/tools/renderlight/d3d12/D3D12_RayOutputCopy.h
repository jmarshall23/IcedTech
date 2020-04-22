// D3D12_RayOutputCopy.h
//

#pragma once

void R_InitRayOutputCopy(int lightmapDimen);
void R_ComputeCopyResultToFinal(ID3D12GraphicsCommandList *commandList);
void R_SaveRaytracingOutput(char *filename);