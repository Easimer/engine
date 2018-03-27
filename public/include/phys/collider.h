#pragma once

#include <phys/phys.h>

namespace phys {

	enum collider_type {
		NONE = 0,
		SPHERE = 1,
		AABB = 2,
		PLANE = 3,
		MAX = 4
	};

	class collider {
	public:
		collider(int type = collider_type::NONE) :
			m_iType(type) {}

		inline int type() const { return m_iType; }

		virtual void transform(const vector3<float>& translation) {};

	protected:
		int m_iType;
	};

	intersect_result intersect(const collider& lhs, const collider& rhs);
}
