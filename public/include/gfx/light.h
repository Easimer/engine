#pragma once

#include <math/vector.h>

namespace gfx {
	struct color_rgba {
		float r, g, b, a;
	};

	struct shader_light {
		math::vector3<float> pos;
		color_rgba color;
	};
}
