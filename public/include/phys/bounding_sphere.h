#pragma once

#include <phys/phys.h>
#include <phys/vector.h>

namespace phys {
	class bounding_sphere {
	public:
		bounding_sphere(const vector3<float> center = vector3<float>(0, 0, 0), float radius = 0) :
			m_vecCenter(center),
			m_flRadius(radius)
		{}

		inline const vector3<float> center() const {
			return m_vecCenter;
		}

		inline float radius() const {
			return m_flRadius;
		}

	protected:
		const vector3<float> m_vecCenter;
		const float m_flRadius;
	};

	intersect_result intersect(const bounding_sphere& lhs, const bounding_sphere& rhs);
}