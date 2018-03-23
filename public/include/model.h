#pragma once

#include <vector>
#include <map>

#define SMD_MAX_BONE_NAME_SIZ 64
#define SMD_MAX_MATERIAL_PATH_SIZ 128

// a model parsed from a SMD file

struct model_bone {
	char szName[SMD_MAX_BONE_NAME_SIZ];
	short iID;
	short iParentID;
};

struct model_bone_state {
	float px, py, pz;
	float rx, ry, rz;
};

struct model_triangle_vertex {
	short iBoneID;
	float px, py, pz;
	float nx, ny, nz;
	float u, v;
};

struct model_triangle {
	size_t iModelMaterial;
	// CLOCKWISE!
	std::vector<model_triangle_vertex> vertices;
};

struct model_material {
	long long int iModelMaterial;
	char szName[SMD_MAX_MATERIAL_PATH_SIZ];
};

struct model_keyframe {
	int iFrame;
	std::vector<std::pair<short, model_bone_state>> bones;
};

struct model {
	std::vector<model_bone> bones;
	std::map<std::string, std::vector<model_keyframe>> animations;
	std::vector<model_material> materials;
	std::vector<model_triangle> triangles;

	long long int iLastMatID = -1;
};
