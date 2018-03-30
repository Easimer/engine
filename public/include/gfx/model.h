#pragma once

#include <math/vector.h>
#include <vector>
#include <map>

namespace gfx {
	const size_t SMD_MAX_BONE_NAME_SIZ = 64;
	const size_t SMD_MAX_MATERIAL_PATH_SIZ = 128;

	struct bone {
		char szName[SMD_MAX_BONE_NAME_SIZ];
		short iID;
		short iParentID;
	};

	struct bone_state {
		math::vector3<float> pos;
		math::vector3<float> rot;
	};

	struct triangle_vertex {
		short iBoneID;
		math::vector3<float> pos;
		math::vector3<float> normal;
		float u, v;
	};

	// CLOCKWISE!
	typedef std::vector<triangle_vertex> triangle;

	struct keyframe {
		int iFrame;
		std::vector<std::pair<short, bone_state>> bones;
	};

	struct model {
		std::vector<bone> bones;
		std::map<std::string, std::vector<keyframe>> animations;
		std::vector<triangle> triangles;
		std::vector<triangle> collider;
		std::string material;
	};
}
