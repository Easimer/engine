#include "stdafx.h"
#include <phys/phys.h>
#include <phys/bounding_sphere.h>

int main(int argc, char** argv)
{

	phys::vector3<float> a(0, 3, 0);
	phys::vector3<float> b(0, 0, 0);
	PRINT_DBG((b - a).length());

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

	return 0;
}
