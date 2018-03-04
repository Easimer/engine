#pragma once

#include <enl/platform.h>
#include <cstdint>

enum emf_file_type {
	EMF_T_VERTEXDATA = 0x01,
	EMF_T_COLLDATA = 0x02,
	EMF_T_SKELETON = 0x03,
	EMF_T_KEYFRAMES = 0x04
};

START_PACK;

struct emf_manifest {
	char szMaterial[256];
	uint64_t nFramerate;
	uint64_t nAnimations;
	//char szAnimations[64][nAnimations];
};

struct emf_hdr {
	uint64_t iID;
	char szMaterial[256];
} PACKED;
END_PACK

struct emf_vertex {
	uint64_t iBone;
	float px, py, pz;
	float nx, ny, nz;
	float u, v;
} PACKED;

struct emf_triangle {
	emf_vertex vertices[3];
} PACKED;

struct emf_vertexdata {
	uint64_t nTriangles;
	emf_triangle triangles[0];
} PACKED;

struct emf_bone {
	uint64_t iBone;
	char szName[64];
} PACKED;

struct emf_skeleton {
	uint64_t nBones;
	emf_bone bones[0];
} PACKED;

struct emf_keyframe {
	uint64_t iBone;
	uint64_t iTime;
	float px, py, pz;
	float rx, ry, rz; // euler engles in radians
};

struct emf_keyframes {
	uint64_t nKeyframes;
	emf_keyframe keyframes[0];
};

END_PACK
