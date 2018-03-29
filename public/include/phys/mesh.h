#pragma once

#include <phys/phys.h>
#include <math/vector.h>
#include <vector>

namespace phys {

	struct ray {
		vector3<float> origin;
		vector3<float> dir;
	};

	struct triangle {
		std::vector<phys::vector3<float>> vertices;

		inline const phys::vector3<float>& operator[](size_t iIndex) const {
			return vertices[iIndex];
		}
	};

	class mesh {

	};

	intersect_result intersect_triangle_ray(const ray& ray, const triangle& tri);
	intersect_result intersect_triangles(const triangle& lhs, const triangle& rhs);

	intersect_result intersect(const mesh&, const mesh&);
}