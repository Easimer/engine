#pragma once

#include "light_base.h"

class c_light_global : public base_light {
public:
	DEC_CLASS(c_light_global, base_light);
	BEGIN_KEYVALUES(c_light_global)
	KEYFIELD(m_vecPos, KV_T_VECTOR3, "position");
	KEYFIELD(m_vecRot, KV_T_VECTOR3, "rotation");
	KEYFIELD(m_colColor, KV_T_RGBA, "color");
	KEYFIELD(m_flAttenConstant, KV_T_FLOAT, "constant");
	KEYFIELD(m_flAttenLinear, KV_T_FLOAT, "linear");
	KEYFIELD(m_flAttenQuadratic, KV_T_FLOAT, "quadratic");
	KEYFIELD(m_flAngle, KV_T_FLOAT, "angle");
	END_KEYVALUES()

	void precache() {}
	void spawn() {
		m_nFilter = ENT_FILTER_LIGHT;
	}

	void get_light(shader_light& l);

private:
	float m_flAngle;
};
