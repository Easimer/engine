#pragma once

#include <math/vector.h>
#include <phys/mesh.h>

namespace phys {
	class object {
	public:
		object(const math::vector3<float>& position = math::vector3<float>(0, 0, 0), const math::vector3<float>& velocity = math::vector3<float>(0, 0, 0)) :
			m_position(position),
			m_velocity(velocity) {}

		void integrate(float delta);

		inline const math::vector3<float> position() const { return m_position; }
		inline const math::vector3<float> velocity() const { return m_velocity; }
		inline phys::mesh& collider() { return m_collider; }

		inline void position(const math::vector3<float>& new_position) { m_position = new_position; }
		inline void velocity(const math::vector3<float>& new_velocity) { m_velocity = new_velocity; }
		inline void collider(const phys::mesh& new_collider) { m_collider = new_collider; }

	protected:
		math::vector3<float> m_position;
		math::vector3<float> m_velocity;
		phys::mesh m_collider;
	};
}
