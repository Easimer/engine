#include "stdafx.h"

int main() {
	const float hmap[] = {
		2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 1, 1, 1, 1, 2, 2,
		2, 2, 1, 0, 0, 1, 2, 2,
		2, 2, 1, 0, 0, 1, 2, 2,
		2, 2, 1, 1, 1, 1, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2,
	};

	elf::terrain_chunk test;
	for (size_t i = 0; i < (test.width) * (test.height); i++) {
		test.heightmap[i] = hmap[i];
	}
	std::unique_ptr<float[]> pBufZ = std::make_unique<float[]>(((test.width + 1) * (test.height + 1)) * sizeof(float));
	memset(pBufZ.get(), 0, ((test.width + 1) * (test.height + 1)) * sizeof(float));
	elf::generate_vertex_z(test, pBufZ.get());
	std::cout.precision(2);
	/*for (size_t y = 0; y < test.height + 1; y++) {
		for (size_t x = 0; x < test.width + 1; x++) {
			float val = pBufZ[y * (test.width + 1) + x];
			if (abs(val) < 0.1)
				val = 0;
			if (abs(val) > 4 || val == -NAN)
				val = -1;
			std::cout << val << '\t';
		}
		std::cout << '\n';
	}*/
	char c;
	std::cin >> c;
	return 0;
}
