#pragma once

#include "vector.h"

struct color_rgba {
	float r, g, b, a;
};

enum shader_light_type {
	SLT_POINT = 0,
	SLT_GLOBAL = 1,
	SLT_DISABLED = 2,
	SLT_MAX = 3
};

struct shader_light {
	int iType;
	vec3 pos;
	color_rgba color;
};
