#pragma once

#include "globals.h"
#include "renderer.h"

#define SET_MODEL(mdlname) m_iModelID = gpGlobals->pRenderer->load_model(mdlname)

class c_base_prop : public base_entity {
public:
	virtual void precache()
	{
		if (m_szModel[0] == '\0') // no model set (i.e. we're precaching all entities)
			return;
		PRECACHE_MODEL(m_szModel);
	}

	virtual void set_model(const char* szFilename)
	{
		if (m_bSpawned)
			return;
		strncpy(m_szModel, szFilename, 128);
		PRECACHE_MODEL(m_szModel);
		SET_MODEL(m_szModel);
	}

	bool is_drawable() { return true; }

	virtual float get_scale() const override {
		return m_flScale;
	}

	virtual void set_scale(float flScale) override {
		m_flScale = flScale;
	}

protected:
	char m_szModel[128] = { 0 };
	float m_flScale = 1.0f;
};
