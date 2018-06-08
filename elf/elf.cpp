#include "stdafx.h"
#include <ifsys/ifsys.h>
#include "..\public\include\elf\elf.h"

void elf::generate_vertex_z(const terrain_chunk & chunk, float * pBufVertexZ) {
	for (int y = 0; y < chunk.height + 1; y++) {
		for (int x = 0; x < chunk.width + 1; x++) {
			float Zn, Zs, Zw, Ze, Zwl, Zel, Znw, Zne, Zsw, Zse;

			// X/Y in the original matrix
			int OriginalX = x;
			int OriginalY = y;
			if (OriginalX == chunk.width)
				OriginalX = chunk.width - 1;
			if (OriginalY == chunk.height)
				OriginalY = chunk.height - 1;

			Zn = chunk.heightmap[(OriginalY - 1) * chunk.width + OriginalX];
			Zs = chunk.heightmap[(OriginalY + 1) * chunk.width + OriginalX];

			Zw = chunk.heightmap[OriginalY * chunk.width + OriginalX - 1];
			Ze = chunk.heightmap[OriginalY * chunk.width + OriginalX + 1];
			Zwl = chunk.heightmap[(OriginalY - 1) * chunk.width + OriginalX - 1];
			Zel = chunk.heightmap[(OriginalY - 1) * chunk.width + OriginalX + 1];

			Znw = chunk.heightmap[(OriginalY - 1) * chunk.width + OriginalX - 1];
			Zne = chunk.heightmap[(OriginalY - 1) * chunk.width + OriginalX + 1];

			int ox, oy;
			ox = x;
			oy = y;
			if (x >= chunk.width)
				ox = chunk.width - 1;
			if (y >= chunk.height)
				oy = chunk.height - 1;

			Zn = chunk.heightmap[(oy - 1) * chunk.width + ox];
			Zs = chunk.heightmap[(oy + 1) * chunk.width + ox];

			Zw = chunk.heightmap[oy * chunk.width + ox - 1];
			Ze = chunk.heightmap[oy * chunk.width + ox + 1];


			if (y != chunk.height) {
				Zsw = chunk.heightmap[(OriginalY + 1) * chunk.width + OriginalX - 1];
				Zse = chunk.heightmap[(OriginalY + 1) * chunk.width + OriginalX + 1];
			} else {
				Zsw = chunk.heightmap[(OriginalY) * chunk.width + OriginalX - 1];
				Zse = chunk.heightmap[(OriginalY) * chunk.width + OriginalX + 1];
			}

			// Corner vertex
			if ((x == 0 || x == chunk.width) && (y == 0 || y == chunk.height)) {
				pBufVertexZ[y * (chunk.width + 1) + x] = chunk.heightmap[OriginalY * chunk.width + OriginalX];
			}
			// Vertical Edge vertex
			else if (x == 0 || x == chunk.width) {
				
				float Z = (Zn + Zs) / 2;
				pBufVertexZ[y * (chunk.width + 1) + x] = Z;
			}
			// Horizontal Edge vertex
			else if (y == 0) {
				float Z = (Zw + Ze) / 2;
				pBufVertexZ[y * (chunk.width + 1) + x] = Z;
			} else if (y == chunk.height) {
				float Z = (Zwl + Zel) / 2;
				PRINT_DBG(Zwl << '+' << Zel << '=' << Z);
				pBufVertexZ[y * (chunk.width + 1) + x] = Z;
			}
			// Inner vertex
			else if (x > 0 && y > 0) {
				float Z = (Znw + Zne + Zsw + Zse) / 4.0f;
				pBufVertexZ[y * (chunk.width + 1) + x] = Z;
			}

			// Print STEP
			PRINT_DBG("==============");
			PRINT_DBG("==============");
			for (size_t y = 0; y < chunk.height + 1; y++) {
				for (size_t x = 0; x < chunk.width + 1; x++) {
					float val = pBufVertexZ[y * (chunk.width + 1) + x];
					if (abs(val) < 0.1)
						val = 0;
					if (abs(val) > 4 || val == -NAN)
						val = -1;
					std::cout << val << '\t';
				}
				std::cout << '\n';
			}
			//DebugBreak();
		}
	}
}
