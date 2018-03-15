#pragma once

#include <vector>
#include <list>
#include <map>
#include <enl/cmdbuf.h>
#include "entsearch.h"

#define ENTSYS_MAX_ENTITIES 1024

#define INVALID_ENTITY 0

enum entsys_update_type {
	ENTSYS_T_SETPOS		= 0,
	ENTSYS_T_SETROT		= 1,
	ENTSYS_T_SETMODEL	= 2,
	ENTSYS_T_SETSCALE	= 3,
	ENTSYS_T_IO			= 4,
	ENTSYS_T_SET_TNAME	= 5,
	ENTSYS_T_CREATE		= 6,
	ENTSYS_T_KILL		= 7,
	ENTSYS_T_MAX		= 8
};

CMDBUF_BEGIN_CMD(entsys_update_t)
	size_t nEntityID;
	entsys_update_type iType;

	vec3 vector;
	char szString[256];
	std::string iszString;
	float flFloat;
CMDBUF_END_CMD(entsys_update_t)

CMDBUF_DEF(entsys_update_cmdbuf, entsys_update_t, 16, true, false);

class entsys {
public:
	entsys();
	~entsys();

	void update_entities();
	void draw_entities();

	void precache_entities();

	void add_entity(base_entity* pEnt);
	void kill_entity(base_entity* pEnt);

	void precache_model(const char* szFilename) { 
		PRINT_DBG("precaching " << szFilename);
		m_vecPrecacheModels.push_back(std::string(szFilename));
	}
	
	base_entity* get_entity(size_t iID);

	void get_entities(std::vector<std::pair<size_t, char[64]>>&) const;

	void send_updates(std::vector<entsys_update_t>&);

private:
	std::list<base_entity*> m_vecEntities;
	std::map<size_t, base_entity*> m_vecEntityIDs;
	size_t m_iNextEntityID = 1;
	// models to be precached
	std::vector<std::string> m_vecPrecacheModels;

	entsys_update_cmdbuf m_update_buf;
};

#define PRECACHE_MODEL(fn) gpGlobals->pEntSys->precache_model(fn)
