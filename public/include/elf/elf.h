#pragma once

#include <cstdint>
#define ENL_LIBRARY
#include <enl/platform.h>

namespace elf {
	const int terrain_chunk_width = 8;
	const int terrain_chunk_height = 8;

	struct terrain_chunk {
		int width = terrain_chunk_width;
		int height = terrain_chunk_height;
		float heightmap[terrain_chunk_width * terrain_chunk_height];
		uint8_t paint[terrain_chunk_width * terrain_chunk_height];
	};

	ENL_EXPORT void generate_vertex_z(const terrain_chunk&, float*);
}
