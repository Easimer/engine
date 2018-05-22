#include "stdafx.h"
#include "prop_common.h"

class c_prop_dynamic : public c_base_prop {
public:
	DEC_CLASS(prop_dynamic, c_base_prop);
	virtual void precache() override;
	virtual void spawn() override;

	BEGIN_KEYVALUES(c_prop_dynamic)
	END_KEYVALUES()
};

REGISTER_ENTITY(c_prop_dynamic, prop_dynamic);

void c_prop_dynamic::precache() {
	BaseClass::precache();
}

void c_prop_dynamic::spawn() {
	BaseClass::spawn();
	precache();
	SetNextThink(DONT_THINK);
	m_nFilter = ENT_FILTER_PROP;
}
