#pragma once
#include "stdafx.h"
#include "entsys.h"

entsys::entsys()
{
	
}

entsys::~entsys()
{

}

void entsys::update_entities()
{
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

void entsys::kill_entity(base_entity * pEnt)
{
	m_vecEntities.remove(pEnt);
	delete pEnt;
}
