#pragma once

#include <math/vector.h>
#include <gfx/defs.h>

namespace gfx {
	struct shader_light {
		math::vector3<float> pos;
		color_rgba color;
	};
}
