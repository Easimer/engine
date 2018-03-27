#include "stdafx.h"
#include <phys/phys.h>
#include <phys/collider.h>

#include <phys/aabb.h>
#include <phys/bounding_sphere.h>
#include <phys/plane.h>

using namespace phys;

#define COLLIDER_IF(lt, rt) if(lhs.type() == lt && rhs.type() == rt)

intersect_result phys::intersect(const collider& lhs, const collider& rhs)
{
	if (lhs.type() == collider_type::NONE || rhs.type() == collider_type::NONE) {
		intersect_result ret;
		ret.hit = false;
		return ret;
	}
	COLLIDER_IF(collider_type::SPHERE, collider_type::SPHERE) {
		const bounding_sphere& lhs_bs = dynamic_cast<const bounding_sphere&>(lhs);
		const bounding_sphere& rhs_bs = dynamic_cast<const bounding_sphere&>(rhs);
		return intersect(lhs_bs, rhs_bs);
	}

	PRINT_ERR("phys::intersect<collider, collider>: intersect method unimplemented for pair <" << lhs.type() << ',' << rhs.type() << '>');

	ASSERT(0);

	return intersect_result();
}
