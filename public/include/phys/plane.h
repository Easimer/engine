#pragma once

#include <phys/phys.h>
#include <phys/vector.h>
#include <phys/bounding_sphere.h>

namespace phys {
	class plane {
	public:
		plane(const vector3<float>& normal = (0, 1, 0), float distance = 0) :
			m_normal(normal),
			m_distance(distance) {}

		plane normalized() const;

		const vector3<float> normal() const { return m_normal; }
		float distance() const { return m_distance; }

	private:
		const vector3<float> m_normal;
		const float m_distance;
	};

	intersect_result intersect(const phys::plane&, const phys::bounding_sphere&);
}
