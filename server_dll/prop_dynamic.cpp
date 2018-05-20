#include "stdafx.h"
#include "prop_common.h"
#include "glm/gtc/quaternion.hpp"

class c_prop_dynamic : public c_base_prop {
public:
	DEC_CLASS(prop_dynamic, c_base_prop);
	virtual void precache() override;
	virtual void spawn() override;

	virtual void think() override;

	BEGIN_KEYVALUES(c_prop_dynamic)
	END_KEYVALUES()
private:
	bool m_bDir = false;
};

REGISTER_ENTITY(c_prop_dynamic, prop_dynamic);

void c_prop_dynamic::precache() {
	BaseClass::precache();
}

void c_prop_dynamic::spawn() {
	BaseClass::spawn();
	precache();
	set_abspos(vec3_origin);
	SetThink(&c_prop_dynamic::think);
	SetNextThink(gpGlobals->curtime + 1);
	m_nFilter = ENT_FILTER_PROP;

	auto pos = get_abspos();
	pos[1] = -5;
	pos[2] = -3.5;
	set_abspos(pos);
}

void c_prop_dynamic::think() {
	SetNextThink(gpGlobals->curtime + 0.5);

	auto pos = get_abspos();
	if(m_bDir)
		pos[1] += 0.5;
	else
		pos[1] -= 0.5;
	if (pos[1] >= 5)
		m_bDir = false;
	if (pos[1] <= -5)
		m_bDir = true;
	set_abspos(pos);
}
