#pragma once

#include "vector.h"

struct color_rgba {
	float r, g, b, a;
};

enum shader_light_type {
	SLT_POINT = 0,
	SLT_GLOBAL = 1,
	SLT_MAX = 2
};

struct shader_light {
	int iType;
	vec3 pos;
	float flConstant;
	float flLinear;
	float flQuadratic;

	color_rgba color;
};
