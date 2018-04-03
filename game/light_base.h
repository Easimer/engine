#pragma once

#include "baseentity.h"
#include <gfx/light.h>

class base_light : public base_entity {
public:
	DEC_CLASS(light_base, base_entity);

	virtual void get_light(gfx::shader_light&) = 0;

	BEGIN_KEYVALUES(base_light)
	END_KEYVALUES()
protected:
	gfx::color_rgba m_colColor;
};
