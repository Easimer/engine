#include "stdafx.h"
#include "prop_common.h"

class c_prop_dynamic : public base_entity {
public:
	DEC_CLASS(prop_dynamic, base_entity);
	void init();
	void precache();
	void spawn();
	bool is_drawable() { return true; }
};

REGISTER_ENTITY(c_prop_dynamic, prop_dynamic);

void c_prop_dynamic::precache()
{
	PRECACHE_MODEL("data/models/triangle.smd");
}

void c_prop_dynamic::spawn()
{
	set_pos(vec3_origin);
	SET_MODEL("data/models/triangle.smd");
	SetNextThink(DONT_THINK);
}
