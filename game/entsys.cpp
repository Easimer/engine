#pragma once
#include "stdafx.h"
#include "entsys.h"
#include "renderer.h"

entsys::entsys()
{
	
}

entsys::~entsys()
{

}

void entsys::update_entities()
{
	RESTRICT_THREAD_LOGIC;
	for (auto& pEnt : m_vecEntities)
	{
		float flNextThink = pEnt->GetNextThink();
		if (flNextThink == DONT_THINK)
			continue;
		if (flNextThink <= gpGlobals->curtime)
		{
			auto pThinkFunc = pEnt->GetThinkFunc();
			if (pThinkFunc)
				(pEnt->*pThinkFunc)();
		}
	}
}

void entsys::draw_entities()
{
	RESTRICT_THREAD_LOGIC;
	std::vector<drawcmd_t> vecDrawCmds;
	for (auto& pEnt : m_vecEntities)
	{
		if (pEnt->is_drawable())
		{
			drawcmd_t c;
			c.iModelID = pEnt->m_iModelID;
			c.vecPosition = pEnt->get_abspos();
			c.flRotation = 0;
			vecDrawCmds.push_back(c);
		}
	}
	if (vecDrawCmds.size() > 0)
	{
		gpGlobals->pRenderer->draw_models(vecDrawCmds);
	}
}

void entsys::precache_entities()
{
	RESTRICT_THREAD_LOGIC;
	for (size_t i = 0; i < gpGlobals->iEntityFactoryDictionaryIndex; i++)
	{
		base_entity* pEnt = gpGlobals->entityFactoryDictionary[i].pFactory->Create();
		pEnt->precache();
		delete pEnt;
	}
	for(auto& fn : m_vecPrecacheModels)
		gpGlobals->pRenderer->load_model(fn.c_str());
}

void entsys::kill_entity(base_entity * pEnt)
{
	RESTRICT_THREAD_LOGIC;
	m_vecEntities.remove(pEnt);
	delete pEnt;
}
