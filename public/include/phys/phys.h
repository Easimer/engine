#pragma once

#include <vector>
#include <phys/vector.h>

namespace phys {
	struct intersect_result {
		bool hit = false;
		std::vector<vector3<float>> points;

		float distance;
	};
}
