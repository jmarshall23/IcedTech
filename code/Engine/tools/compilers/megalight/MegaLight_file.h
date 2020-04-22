// MegaLight_file.h
//


#pragma once

/*
========================
MegaLight Intermediate file format

This format is designed to pass off geometry to the renderlight tool.
========================
*/

//
// MegaLightFileHeader_t
//
struct MegaLightFileHeader_t {
	int numVertexes;
	int numIndexes;
	int numLights;
};

//
// MegaLight_t
//
struct MegaLight_t {
#ifdef RENDER_LIGHT_TOOL
	float origin[3];
	float color[3];
#else
	idVec3 origin;
	idVec3 color;
#endif
	int radius;
	bool isSkyLight;
};

//
// MegaLightVertex_t
//
struct MegaLightVertex_t {
#ifdef RENDER_LIGHT_TOOL
	float xyz[3];
	float st[2];
	float normal[3];
#else
	idVec3 xyz;
	idVec2 st;
	idVec3 normal;
#endif
	bool isMegaTextureGeometry;
};

//
// MegaLightGeometry_t
//
#ifndef RENDER_LIGHT_TOOL
struct MegaLightGeometry_t {
	rvmListSTL<MegaLightVertex_t> vertexes;
	rvmListSTL<unsigned int> indexes;
	rvmListSTL<MegaLight_t> lights;
};
#endif