#pragma once

#include <phys/phys.h>
#include <math/vector.h>
#include <vector>
#include <gfx/model.h>

namespace phys {

	struct ray {
		math::vector3<float> origin;
		math::vector3<float> dir;
	};

	struct triangle {
		std::vector<math::vector3<float>> vertices;

		inline const math::vector3<float>& operator[](size_t iIndex) const {
			return vertices[iIndex];
		}
	};

	typedef std::vector<gfx::triangle> mesh;

	intersect_result intersect_triangle_ray(const ray& ray, const triangle& tri);
	intersect_result intersect_triangles(const triangle& lhs, const triangle& rhs);

	intersect_result intersect(const mesh&, const mesh&);
}