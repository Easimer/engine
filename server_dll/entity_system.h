#pragma once

#include <vector>
#include "base_entity.h"

using entity_id = size_t;
using entity_handle = size_t;

class entity_system {
public:
	entity_handle add_entity(base_entity* ent);
	base_entity* get_entity(entity_handle h);
	void remove_entity(base_entity* ent);
	void remove_entity(entity_handle h);

	void update_entities();

	const std::vector<base_entity*>& ptr() const { return m_entities; }

private:
	std::vector<base_entity*> m_entities;
};
