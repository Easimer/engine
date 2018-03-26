#include "stdafx.h"
#include <phys/phys.h>
#include <phys/aabb.h>
#include <phys/bounding_sphere.h>
#include <phys/object.h>

int main(int argc, char** argv)
{
	phys::bounding_sphere s1(phys::vector3<float>(0, 0, 0), 1);
	phys::bounding_sphere s2(phys::vector3<float>(0, 3, 0), 1);
	phys::bounding_sphere s3(phys::vector3<float>(0, 0, 2), 1);
	phys::bounding_sphere s4(phys::vector3<float>(1, 0, 0), 1);

	auto res1 = phys::intersect(s1, s2);
	auto res2 = phys::intersect(s1, s3);
	auto res3 = phys::intersect(s1, s4);

	PRINT_DBG("s1-s2: " << res1.hit << ", dist: " << res1.distance);
	PRINT_DBG("s1-s3: " << res2.hit << ", dist: " << res2.distance);
	PRINT_DBG("s1-s4: " << res3.hit << ", dist: " << res3.distance);

	phys::aabb aabb1(phys::vector3<float>(0, 0, 0), phys::vector3<float>(1, 1, 1));
	phys::aabb aabb2(phys::vector3<float>(1, 1, 1), phys::vector3<float>(2, 2, 2));
	phys::aabb aabb3(phys::vector3<float>(1, 0, 0), phys::vector3<float>(2, 1, 1));
	phys::aabb aabb4(phys::vector3<float>(0, 0, -2), phys::vector3<float>(1, 1, -1));
	phys::aabb aabb5(phys::vector3<float>(0, 0.5f, 0), phys::vector3<float>(1, 1.5f, 1));

	res1 = phys::intersect(aabb1, aabb2);
	res2 = phys::intersect(aabb1, aabb3);
	res3 = phys::intersect(aabb1, aabb4);
	auto res4 = phys::intersect(aabb1, aabb5);

	PRINT_DBG("aabb1-aabb2: " << res1.hit << ", dist: " << res1.distance);
	PRINT_DBG("aabb1-aabb3: " << res2.hit << ", dist: " << res2.distance);
	PRINT_DBG("aabb1-aabb4: " << res3.hit << ", dist: " << res3.distance);
	PRINT_DBG("aabb1-aabb5: " << res4.hit << ", dist: " << res4.distance);

	phys::object test(phys::vector3<float>(0, 1, 0), phys::vector3<float>(1, 2, 3));

	test.integrate(20);

	PRINT_DBG(test.position());
	PRINT_DBG(test.velocity());

	return 0;
}
