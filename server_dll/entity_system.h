#pragma once

#include <vector>
#include <array>
#include "base_entity.h"
#include <net/networking.h>

using entity_id = size_t;
using entity_handle = size_t;

class entity_system {
public:
	entity_handle add_entity(base_entity* ent);
	base_entity* get_entity(entity_handle h);
	base_entity* get_entity_by_edict(size_t edict);
	void remove_entity(base_entity* ent);
	void remove_entity(entity_handle h);

	void update_entities();

	const std::vector<base_entity*>& ptr() const { return m_entities; }

	entity_handle get_free_edict(bool bPlayer) const;

private:
	std::vector<base_entity*> m_entities;
	std::map<size_t, size_t> m_map_edicts_entities;
	std::array<bool, net::max_edicts> m_edicts;
};
