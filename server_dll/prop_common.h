#pragma once

#include "globals.h"
#include "base_entity.h"
//#include "igfx.h"

class c_base_prop : public base_entity {
public:
	virtual void precache()
	{
		if (m_szModel[0] == '\0') // no model set (i.e. we're precaching all entities)
			return;
	}

	virtual void set_model(const char* szFilename)
	{
		if (m_bSpawned)
			return;
		strncpy(m_szModel, szFilename, 128);
	}

	bool is_drawable() { return true; }

	virtual float get_scale() const override {
		return m_flScale;
	}

	virtual void set_scale(float flScale) override {
		m_flScale = flScale;
	}

	/// Is the prop static?
	/// Static props have the transformation matrix precached.
	virtual bool is_static() const {
		return false;
	}

protected:
	char m_szModel[128] = { 0 };
	float m_flScale = 1.0f;
};
