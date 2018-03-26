#include "stdafx.h"
#include <print.h>
#include <phys/simulation.h>

using namespace phys;

size_t phys::simulation::add_object(const object& obj)
{
	m_objects.push_back(obj);
	return m_objects.size() - 1;
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
