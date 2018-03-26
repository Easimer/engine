#include "stdafx.h"
#include <phys/simulation.h>

using namespace phys;

void phys::simulation::add_object(const object& obj)
{
	m_objects.push_back(obj);
}

void phys::simulation::simulate(float delta)
{
	for (size_t i = 0; i < m_objects.size(); i++) {
		m_objects[i].integrate(delta);
	}
}
