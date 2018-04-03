#include "stdafx.h"
#include "light_global.h"

#ifdef PLAT_DEBUG
#include "devgui.h"
#include "prop_common.h"
#endif

REGISTER_ENTITY(c_light_global, light_global);

void c_light_global::precache()
{
#ifdef PLAT_DEBUG
	m_iModelID = gpGlobals->pRenderer->load_model("data/models/debug/directional_light.emf");
#endif
}

bool c_light_global::is_drawable() {
#ifdef PLAT_DEBUG
	//return gpGlobals->pDevGUI->m_bDrawGlobalLightGizmo;
#endif
	return false;
}

void c_light_global::spawn()
{
	BaseClass::spawn();
	precache();
	m_nFilter = ENT_FILTER_LIGHT_GLOBAL;
}

void c_light_global::get_light(gfx::shader_light & l)
{
	l.pos = m_vecRot;
	l.color = m_colColor;
	l.iType = gfx::shader_light_type::SLT_GLOBAL;
}
