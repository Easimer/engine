#include "stdafx.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define FEQ(v1, v2) (std::abs(v1 - v2) < math::EPSILON)

void test_math() {
	// No arguments
	math::vector3<float> test1;
	PRINT_DBG(test1);
	ASSERT(FEQ(test1.x(), 0));
	ASSERT(FEQ(test1.y(), 0));
	ASSERT(FEQ(test1.z(), 0));
	// Arguments
	math::vector3<float> test2(-1, 5, 10.10);
	PRINT_DBG(test2);
	ASSERT(FEQ(test2.x(), -1));
	ASSERT(FEQ(test2.y(), 5));
	ASSERT(FEQ(test2.z(), 10.10));
	// Index operator
	math::vector3<float> test3(4, 5, 6);
	PRINT_DBG(test3);
	ASSERT(FEQ(test3[0], 4));
	ASSERT(FEQ(test3[1], 5));
	ASSERT(FEQ(test3[2], 6));
	// Addition
	math::vector3<float> test4 = math::vector3<float>(-54, 60, 30.30) + math::vector3<float>(42.5, -6, 5);
	PRINT_DBG(test4);
	ASSERT(FEQ(test4.x(), -11.5));
	ASSERT(FEQ(test4.y(), 54));
	ASSERT(FEQ(test4.z(), 35.30));
	// Subtraction
	math::vector3<float> test5 = math::vector3<float>(-54, 60, 30.30) - math::vector3<float>(42.5, -6, 5);
	PRINT_DBG(test5);
	ASSERT(FEQ(test5.x(), -96.5));
	ASSERT(FEQ(test5.y(), 66));
	ASSERT(FEQ(test5.z(), 25.30));
	// Normalization
	math::vector3<float> test6(40, 50, 60);
	test6.normalize();
	ASSERT(FEQ(test6.x(), 0.455842306));
	ASSERT(FEQ(test6.y(), 0.569802882));
	ASSERT(FEQ(test6.z(), 0.683763459));
	// Scalar multiplication
	math::vector3<float> test7 = 4.0f * math::vector3<float>(10, 20, 30);
	ASSERT(FEQ(test7.x(), 40));
	ASSERT(FEQ(test7.y(), 80));
	ASSERT(FEQ(test7.z(), 120));
	// Scalar division
	math::vector3<float> test8 = math::vector3<float>(10, 20, 30) / 2.0f;
	ASSERT(FEQ(test8.x(), 5));
	ASSERT(FEQ(test8.y(), 10));
	ASSERT(FEQ(test8.z(), 15));
	// Min, max
	math::vector3<float> test9(20, 30, 10);
	ASSERT(FEQ(test9.max(), 30));
	ASSERT(FEQ(test9.min(), 10));
}

gfx::triangle pt2mt(const phys::triangle& t) {
	gfx::triangle ret;
	for (const auto& v : t.vertices) {
		gfx::triangle_vertex vtx;
		vtx.pos[0] = v.x();
		vtx.pos[1] = v.y();
		vtx.pos[2] = v.z();
		ret.push_back(vtx);
	}
	return ret;
}

int main(int argc, char** argv)
{
	test_math();

	phys::object test(math::vector3<float>(0, 1, 0), math::vector3<float>(1, 2, 3));

	test.integrate(20);

	PRINT_DBG("Physics integration test:");
	PRINT_DBG("Position: " << test.position());
	PRINT_DBG("Velocity: " << test.velocity());
	ASSERT(test.position() == math::vector3<float>(20, 41, 60));
	ASSERT(test.velocity() == math::vector3<float>(1, 2, 3));

	phys::triangle t1;
	phys::triangle t2;
	t1.vertices.push_back(math::vector3<float>(-1, 0, 0));
	t1.vertices.push_back(math::vector3<float>(1, 0, 0));
	t1.vertices.push_back(math::vector3<float>(0, 1, 0));

	phys::ray r1;
	r1.origin = math::vector3<float>(0, 0.5, 1);
	r1.dir = math::vector3<float>(0, 0, -1);

	auto res_tri = phys::intersect_triangle_ray(r1, t1);

	PRINT_DBG("Triangle-ray intersection test:");
	PRINT_DBG("Hit: " << res_tri.hit);
	PRINT_DBG("Distance: " << res_tri.distance);

	ASSERT(res_tri.hit);

	t1.vertices.clear();
	t1.vertices.push_back(math::vector3<float>(0, 0, 0));
	t1.vertices.push_back(math::vector3<float>(2, 0, 0));
	t1.vertices.push_back(math::vector3<float>(1, 2, 0));
	t2.vertices.push_back(math::vector3<float>(1, 1, 0));
	t2.vertices.push_back(math::vector3<float>(2, -1, 0));
	t2.vertices.push_back(math::vector3<float>(3, 1, 0));

	res_tri = phys::intersect_triangles(t1, t2);
	PRINT_DBG("Triangle-triangle intersection test:");
	PRINT_DBG("Hit: " << res_tri.hit);
	ASSERT(res_tri.hit);

	phys::mesh a = { pt2mt(t1) };
	phys::mesh b = { pt2mt(t2) };
	auto res_mesh = phys::intersect(a, b);
	PRINT_DBG("Mesh-mesh intersection test:");
	PRINT_DBG("Hit: " << res_mesh.hit);
	ASSERT(res_mesh.hit);

	return 0;
}
