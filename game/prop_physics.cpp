#include "stdafx.h"
#include "prop_common.h"
#include "prop_physics.h"
#include <phys/mesh.h>

REGISTER_ENTITY(c_prop_physics, prop_physics);

c_prop_physics::c_prop_physics()
{
	if (gpGlobals->pPhysSimulation) {
		phys::object obj;
		m_iPhysicsHandle = gpGlobals->pPhysSimulation->add_object(obj);
	}
}

c_prop_physics::~c_prop_physics()
{
	if (m_iPhysicsHandle) {

	}
}

void c_prop_physics::spawn()
{
	BaseClass::spawn();
	precache();
	SET_MODEL(m_szModel);
	m_nFilter = ENT_FILTER_PROP;

	SetThink(&c_prop_physics::think);
	SetNextThink(gpGlobals->curtime + 1 / 64);
}

void c_prop_physics::precache()
{
	BaseClass::precache();
}

void c_prop_physics::think()
{
	auto phys_object = gpGlobals->pPhysSimulation->get_object(m_iPhysicsHandle);
	auto newpos = phys_object.position();
	// TODO: write vector(math::vector3<float>) constructor
	m_vecPos[0] = newpos.x();
	m_vecPos[1] = newpos.y();
	m_vecPos[2] = newpos.z();
	SetNextThink(gpGlobals->curtime + 1 / 64);

	auto velocity = phys_object.velocity();
	m_vecVelocity[0] = velocity.x();
	m_vecVelocity[1] = velocity.y();
	m_vecVelocity[2] = velocity.z();
}
