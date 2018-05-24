#include "stdafx.h"
#include "entity_system.h"
#include <algorithm>
#include <net/networking.h>

entity_handle entity_system::add_entity(base_entity * ent) {
	auto pos = std::find(m_entities.cbegin(), m_entities.cend(), ent);
	if (pos != std::end(m_entities)) {
		return pos - m_entities.cbegin();
	}
	m_entities.push_back(ent);
	entity_handle hEnt = m_entities.size() - 1;
	if (!ent->networked())
		return hEnt;
	entity_handle e = get_free_edict(ent->is_player());
	ent->edict(e);
	m_edicts[e] = true;
	m_map_edicts_entities.emplace(e, m_entities.size() - 1);
	PRINT_DBG("Entity " << ent->get_classname() << " assigned to edict " << e);
	return hEnt;
}

base_entity * entity_system::get_entity(entity_handle h) {
	if(h >= m_entities.size())
		return nullptr;
	return m_entities[h];
}

base_entity * entity_system::get_entity_by_edict(size_t edict) {
	if (!m_map_edicts_entities.count(edict))
		return nullptr;
	size_t i = m_map_edicts_entities[edict];
	if (i >= m_entities.size())
		return nullptr;
	base_entity* pEnt = m_entities[i];
	return pEnt;
}

void entity_system::remove_entity(base_entity * ent) {
	if (!ent)
		return;
	for (auto& pEnt : m_entities) {
		if (pEnt == ent) {
			pEnt = nullptr;
			delete ent;
		}
	}
}

void entity_system::remove_entity(entity_handle h) {
	if (h >= m_entities.size())
		return;
	auto& pEnt = m_entities[h];
	if (!pEnt)
		return;
	delete pEnt;
	pEnt = nullptr;
}

void entity_system::update_entities() {
	for (auto pEnt : m_entities) {
		if (pEnt->get_next_think() <= gpGlobals->curtime) {
			auto tfun = pEnt->think_func();
			if (tfun) {
				(pEnt->*tfun)();
			}
		}
	}
}

entity_handle entity_system::get_free_edict(bool bPlayer) const {
	entity_handle hEnt;
	if (bPlayer)
		hEnt = 1;
	else
		hEnt = net::max_players + 1;
	for (; hEnt < net::max_edicts; hEnt++) {
		if (!m_edicts[hEnt])
			return hEnt;
	}
	return net::invalid_edict;
}
