#include "stdafx.h"
#include <phys/bounding_sphere.h>
#include <print.h>

using namespace phys;

intersect_result phys::intersect(const bounding_sphere & lhs, const bounding_sphere & rhs)
{
	intersect_result res;
	float rad_distance = lhs.radius() + rhs.radius();
	float cnt_distance = (lhs.center() - rhs.center()).length();
	float hit_distance = cnt_distance - rad_distance;

	PRINT_DBG(lhs.center() << ',' << rhs.center());

	if (cnt_distance < rad_distance) {
		res.hit = true;
		res.distance = hit_distance;
	} else {
		res.hit = false;
		res.distance = hit_distance;
	}

	return res;
}
