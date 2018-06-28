#pragma once

#include <cstdint>
#include <type_traits>
#include <enl/platform.h>

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

	START_PACK
	struct color_rgb888_packed {
		uint8_t r, g, b;
	} PACKED;
	END_PACK

	struct color_rgba {
		float r, g, b, a;
	};

	START_PACK
	struct color_rgb565 {
		uint16_t r : 5;
		uint16_t g : 6;
		uint16_t b : 5;
	} PACKED;
	END_PACK
}
