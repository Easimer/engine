#include "stdafx.h"
#include <phys/phys.h>
#include <phys/object.h>
#include <phys/mesh.h>

int main(int argc, char** argv)
{
	phys::object test(math::vector3<float>(0, 1, 0), math::vector3<float>(1, 2, 3));

	test.integrate(20);

	PRINT_DBG(test.position());
	PRINT_DBG(test.velocity());

	phys::triangle t1;
	phys::triangle t2;
	t1.vertices.push_back(math::vector3<float>(-1, 0, 0));
	t1.vertices.push_back(math::vector3<float>(1, 0, 0));
	t1.vertices.push_back(math::vector3<float>(0, 1, 0));

	phys::ray r1;
	r1.origin = math::vector3<float>(0, 0.5, 1);
	r1.dir = math::vector3<float>(0, 0, -1);

	auto res = phys::intersect_triangle_ray(r1, t1);
	PRINT_DBG("Raycast: hit(" << res.hit << "), dist: " << res.distance);

	t1.vertices.clear();
	t1.vertices.push_back(math::vector3<float>(0, 0, 0));
	t1.vertices.push_back(math::vector3<float>(2, 0, 0));
	t1.vertices.push_back(math::vector3<float>(1, 2, 0));
	t2.vertices.push_back(math::vector3<float>(1, 1, 0));
	t2.vertices.push_back(math::vector3<float>(2, -1, 0));
	t2.vertices.push_back(math::vector3<float>(3, 1, 0));

	res = phys::intersect_triangles(t1, t2);
	PRINT_DBG("2tri: hit(" << res.hit << "), dist: " << res.distance);

	return 0;
}
