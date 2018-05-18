#pragma once

#include "globals.h"
#include "base_entity.h"
//#include "igfx.h"

class c_base_prop : public base_entity {
public:
	virtual ~c_base_prop() {}
	virtual void precache() {
		if (m_szModel[0] == '\0') // no model set (i.e. we're precaching all entities)
			return;
	}

	virtual void model(const char* szModel) {
		if (m_bSpawned)
			return;
		strncpy(m_szModel, szModel, 128);
	}

	virtual const char* model() const {
		return m_szModel;
	}

	virtual bool is_drawable() override {
		return true;
	}

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

	virtual bool networked() const {
		return true;
	}

protected:
	char m_szModel[128] = { 0 };
	float m_flScale = 1.0f;
};
