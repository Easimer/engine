#pragma once

#include <vector>
#include <math/vector.h>

namespace phys {
	struct intersect_result {
		bool hit = false;
		math::vector3<float> point;

		float distance;

		operator bool() const {
			return hit;
		}
	};
}
