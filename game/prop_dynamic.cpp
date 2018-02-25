#include "stdafx.h"
#include "prop_common.h"
#include "glm/gtc/quaternion.hpp"

class c_prop_dynamic : public c_base_prop {
public:
	DEC_CLASS(prop_dynamic, c_base_prop);
	void precache();
	void spawn();

	void think();
};

REGISTER_ENTITY(c_prop_dynamic, prop_dynamic);

void c_prop_dynamic::precache()
{
	BaseClass::precache();
}

void c_prop_dynamic::spawn()
{
	BaseClass::spawn();
	precache();
	set_abspos(vec3_origin);
	SET_MODEL(m_szModel);
	SetThink(&c_prop_dynamic::think);
	SetNextThink(0);
}

void c_prop_dynamic::think()
{
	auto rot = get_relrot();
	rot[1] += 1;
	set_rotation(rot);
	SetNextThink(gpGlobals->curtime + 0.1);
}
