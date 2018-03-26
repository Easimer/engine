#pragma once

#include <phys/vector.h>

namespace phys {
	class object {
	public:
		object(const vector3<float>& position = (0, 0, 0), const vector3<float>& velocity = (0, 0, 0)) :
			m_position(position),
			m_velocity(velocity) {}

		void integrate(float delta);

		inline const vector3<float> position() const { return m_position; }
		inline const vector3<float> velocity() const { return m_velocity; }

	protected:
		vector3<float> m_position;
		vector3<float> m_velocity;
	};
}
