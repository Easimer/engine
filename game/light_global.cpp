#include "stdafx.h"
#include "light_global.h"

REGISTER_ENTITY(c_light_global, light_global);

void c_light_global::get_light(shader_light & l)
{
	l.pos = get_abspos();
	l.color = m_colColor;
	l.iType = SLT_GLOBAL;
}
