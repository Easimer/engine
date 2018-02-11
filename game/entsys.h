#pragma once

#include <list>

#define ENTSYS_MAX_ENTITIES 1024

#define ESPAGE_SIZ 4096		// page size / max entity size
#define ESPAGE_COUNT ENTSYS_MAX_ENTITIES

typedef unsigned char entsys_page[ESPAGE_SIZ];

class entsys {
public:
	entsys();
	~entsys();

	void update_entities();
	void draw_entities();

	void add_entity(base_entity* pEnt) { m_vecEntities.push_back(pEnt); }
	void kill_entity(base_entity* pEnt);

private:
	std::list<base_entity*> m_vecEntities;
};
