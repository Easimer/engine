#pragma once

#include <cstdint>
#include <type_traits>

namespace gfx {
	// Model handle
	using model_id = uint32_t;
	// Shader handle
	using shader_id = int;
	// Material handle
	using material_id = size_t;

	enum texture_type {
		TEX_DIFFUSE		= 0,
		TEX_NORMAL		= 1,
		TEX_SPECULAR	= 2,
		TEX_SELFILLUM	= 3,
		TEX_MAX			= 4,
		TEX_DEPTH
	};
}
