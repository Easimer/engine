#pragma once

#include "prop_common.h"

class c_prop_physics : public c_base_prop {
public:
	DEC_CLASS(prop_physics, c_base_prop);
	c_prop_physics();
	~c_prop_physics();
	void spawn() override;
	void precache() override;

	void think();

	inline size_t physics_handle() const { return m_iPhysicsHandle; }

	BEGIN_KEYVALUES(c_prop_physics)
		KEYFIELD(m_iPhysicsHandle, KV_T_INT, "physics_handle");
		KEYFIELD(m_vecVelocity, KV_T_VECTOR3, "velocity");
	END_KEYVALUES()
private:
	size_t m_iPhysicsHandle = 0;

	vec3 m_vecVelocity;
};
