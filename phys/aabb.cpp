#include "stdafx.h"
#include <phys/phys.h>
#include <phys/aabb.h>

using namespace phys;

intersect_result phys::intersect(const aabb & lhs, const aabb & rhs)
{
	intersect_result ret;
	ret.hit = false;
	ret.distance = 0;

	auto distances1 = rhs.min() - lhs.max();
	auto distances2 = lhs.min() - rhs.max();
	auto distances = phys::max(distances1, distances2);

	float flMaxDistance = distances.max();

	ret.hit = flMaxDistance < 0;
	ret.distance = flMaxDistance;

	return ret;
}
