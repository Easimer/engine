#pragma once

#include <phys/vector.h>
#include <phys/collider.h>

namespace phys {
	class object {
	public:
		object(const vector3<float>& position = vector3<float>(0, 0, 0), const vector3<float>& velocity = vector3<float>(0, 0, 0)) :
			m_position(position),
			m_velocity(velocity) {}

		void integrate(float delta);

		inline const vector3<float> position() const { return m_position; }
		inline const vector3<float> velocity() const { return m_velocity; }
		inline collider& collider() { return m_collider; }

		inline void position(const vector3<float>& new_position) { m_position = new_position; }
		inline void velocity(const vector3<float>& new_velocity) { m_velocity = new_velocity; }
		inline void collider(const phys::collider& new_collider) { m_collider = new_collider; }

	protected:
		vector3<float> m_position;
		vector3<float> m_velocity;
		phys::collider m_collider;
	};
}
