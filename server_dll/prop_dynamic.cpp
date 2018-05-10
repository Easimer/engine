#include "stdafx.h"
#include "prop_common.h"
#include "glm/gtc/quaternion.hpp"

class c_prop_dynamic : public c_base_prop {
public:
	DEC_CLASS(prop_dynamic, c_base_prop);
	void precache();
	void spawn();

	BEGIN_KEYVALUES(c_prop_dynamic)
	END_KEYVALUES()
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
	SetNextThink(DONT_THINK);
	m_nFilter = ENT_FILTER_PROP;
}
