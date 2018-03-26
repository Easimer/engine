#include "stdafx.h"
#include <print.h>
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

const phys::object & phys::simulation::get_object(size_t iIndex) const
{
	if (iIndex < size()) {
		return m_objects[iIndex];
	}
	else {
		PRINT_ERR("phys::simulation::get_object: requested non-existent object");
		return m_empty;
	}
}
