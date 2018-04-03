#include "stdafx.h"
#include "light_base.h"

class c_light_point : public base_light {
public:
	DEC_CLASS(light_point, base_light);
	BEGIN_KEYVALUES(c_light_point)
	KEYFIELD(m_vecPos, KV_T_VECTOR3, "position");
	KEYFIELD(m_vecRot, KV_T_VECTOR3, "rotation");
	KEYFIELD(m_colColor, KV_T_RGBA, "color");
	END_KEYVALUES()

	virtual void precache() {}

	virtual void spawn() {
		BaseClass::spawn();
		m_nFilter = ENT_FILTER_LIGHT_LOCAL;
	}

	void get_light(gfx::shader_light& l);
};

REGISTER_ENTITY(c_light_point, light_point);

void c_light_point::get_light(gfx::shader_light& l) {
	l.pos = get_abspos();
	l.color = m_colColor;
	l.iType = gfx::shader_light_type::SLT_POINT;
}