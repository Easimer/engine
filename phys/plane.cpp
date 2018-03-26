#include "stdafx.h"
#include <phys/plane.h>
using namespace phys;

plane phys::plane::normalized() const
{
	float flMagnitude = m_normal.length();

	return plane(m_normal / flMagnitude, m_distance / flMagnitude);
}

intersect_result phys::intersect(const phys::plane& p, const phys::bounding_sphere& s)
{
	intersect_result ret;
	float flDistanceCenter = std::fabs(dot<float>(p.normal(), s.center()) + p.distance());
	float flDistanceSphere = flDistanceCenter - s.radius();

	ret.distance = flDistanceSphere;
	ret.hit = flDistanceSphere < 0;

	return ret;
}
