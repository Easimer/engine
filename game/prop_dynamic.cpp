#include "stdafx.h"

class c_prop_dynamic : public base_entity {
public:
	DEC_CLASS(prop_dynamic, base_entity);
	void init();
	void precache();
	void spawn();
	bool is_drawable() { return true; }
};

REGISTER_ENTITY(c_prop_dynamic, prop_dynamic);

void c_prop_dynamic::init()
{
	set_pos(vec3_origin);
}

void c_prop_dynamic::precache()
{
	
}

void c_prop_dynamic::spawn()
{

}
