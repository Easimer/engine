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
	for (size_t i = 1; i < m_objects.size(); i++) {
		m_objects[i].integrate(delta);
	}
	for (size_t i = 1; i < m_objects.size(); i++) {
		for (size_t j = i + 1; j < m_objects.size(); j++) {
			auto lcoll = m_objects[i].collider();
			auto rcoll = m_objects[j].collider();
			if (lcoll.type() == collider_type::NONE)
				continue;
			if (rcoll.type() == collider_type::NONE)
				continue;
			intersect_result res = intersect(lcoll, rcoll);
			if (res) {
				m_objects[i].velocity(-1.0f * m_objects[i].velocity());
				m_objects[j].velocity(-1.0f * m_objects[i].velocity());
			}
		}
	}
}

phys::object& phys::simulation::get_object(size_t iIndex)
{
	if (iIndex < size()) {
		return m_objects[iIndex];
	}
	else {
		PRINT_ERR("phys::simulation::get_object: requested non-existent object");
		return m_empty;
	}
}
