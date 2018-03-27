#pragma once

#include <phys/phys.h>
#include <phys/vector.h>

namespace phys {
	class aabb : public collider {
	public:
		aabb(const vector3<float>& min = (0, 0, 0), const vector3<float>& max = (0, 0, 0)) :
			collider(collider_type::AABB),
			m_min(min),
			m_max(max) {}

		inline const vector3<float> min() const {
			return m_min;
		}

		inline const vector3<float> max() const {
			return m_max;
		}

	protected:
		const vector3<float> m_min, m_max;
	};

	intersect_result intersect(const aabb& lhs, const aabb& rhs);
}
