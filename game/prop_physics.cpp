#include "stdafx.h"
#include "prop_common.h"

class c_prop_physics : public c_base_prop {
public:
	DEC_CLASS(prop_physics, c_base_prop);
	~c_prop_physics();
	void spawn() override;
	void precache() override;

	BEGIN_KEYVALUES(c_prop_physics)
	END_KEYVALUES()
private:
	size_t m_iPhysicsHandle = 0;
};

REGISTER_ENTITY(c_prop_physics, prop_physics);

c_prop_physics::~c_prop_physics()
{
	if (m_iPhysicsHandle) {

	}
}

void c_prop_physics::spawn()
{
	BaseClass::spawn();
	precache();
	set_abspos(vec3_origin);
	SET_MODEL(m_szModel);
	SetNextThink(DONT_THINK);
	m_nFilter = ENT_FILTER_PROP;

	if (gpGlobals->pPhysSimulation) {
		phys::vector3<float> pos(m_vecPos[0], m_vecPos[1], m_vecPos[2]);
		phys::object obj(pos);
		m_iPhysicsHandle = gpGlobals->pPhysSimulation->add_object(obj);
	}
}

void c_prop_physics::precache()
{
	BaseClass::precache();
}
