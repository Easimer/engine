#include "stdafx.h"
#include "light_base.h"

class c_light_point : public base_light {
public:
	DEC_CLASS(c_light_point, base_light);
	BEGIN_KEYVALUES(c_light_point)
	KEYFIELD(m_vecPos, KV_T_VECTOR3, "position");
	KEYFIELD(m_vecRot, KV_T_VECTOR3, "rotation");
	KEYFIELD(m_colColor, KV_T_RGBA, "color");
	KEYFIELD(m_flAttenConstant, KV_T_FLOAT, "constant");
	KEYFIELD(m_flAttenLinear, KV_T_FLOAT, "linear");
	KEYFIELD(m_flAttenQuadratic, KV_T_FLOAT, "quadratic");
	END_KEYVALUES()

	c_light_point() {
		m_flAttenConstant = 0;
		m_flAttenLinear = 0;
		m_flAttenQuadratic = 1;
	}

	void precache() {}
	void spawn() {
		BaseClass::spawn();
		m_nFilter = ENT_FILTER_LIGHT;
	}

	void get_light(shader_light& l);
};

REGISTER_ENTITY(c_light_point, light_point);

void c_light_point::get_light(shader_light& l) {
	l.pos = get_abspos();
	l.color = m_colColor;
	l.flConstant = m_flAttenConstant;
	l.flLinear= m_flAttenLinear;
	l.flQuadratic = m_flAttenQuadratic;
	l.iType = SLT_POINT;
}