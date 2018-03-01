#pragma once
#include "stdafx.h"
#include "entsys.h"
#include "renderer.h"
#include "icamera.h"
#include <algorithm>

entsys::entsys()
{
	
}

entsys::~entsys()
{
	//for (auto& ent : m_vecEntities)
	//{
	//	delete ent;
	//}
}

void entsys::update_entities()
{
	RESTRICT_THREAD_LOGIC;

	// process entsys updates
	if (!m_update_buf.is_empty())
	{
		size_t nUpdates = 0;
		entsys_update_t* pUpdate = nullptr;
		if (m_update_buf.begin_read(&pUpdate, &nUpdates))
		{
			if (nUpdates != 0)
			{
				PRINT_DBG("entsys: received " << nUpdates << " updates");
				while (nUpdates--)
				{
					auto pEnt = get_entity(pUpdate->nEntityID);
					if (pEnt)
					{
						switch (pUpdate->iType)
						{
						case ENTSYS_T_SETPOS:
							pEnt->set_abspos(pUpdate->vector);
							break;
						case ENTSYS_T_SETROT:
							break;
						case ENTSYS_T_SETMODEL:
							break;
						default:
							PRINT_DBG("entsys::update: invalid update type " << pUpdate->iType);
							break;
						}
					}
					else
					{
						PRINT_DBG("entsys::update: attempted to update entity with invalid handle " << pUpdate->nEntityID);
					}
					pUpdate++;
				}
			}
			m_update_buf.end_read();
		}
	}

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

struct drawcmdsort {
	inline bool operator()(const std::pair<drawcmd_t, float>& a, const std::pair<drawcmd_t, float>& b)
	{
		return a.second > b.second;
	}
};

void entsys::draw_entities()
{
	RESTRICT_THREAD_LOGIC;
	
	std::vector<std::pair<drawcmd_t, float>> vecDrawCmdsDists;
	vec3 vecCamera = gpGlobals->pCamera->get_pos();
	for (auto& pEnt : m_vecEntities)
	{
		if (pEnt->is_drawable())
		{
			drawcmd_t c;
			c.iModelID = pEnt->m_iModelID;
			c.vecPosition = pEnt->get_abspos();
			c.matRotation = pEnt->get_rotation_matrix();
			c.flScale = pEnt->get_scale();

			float flDist = (vecCamera - pEnt->get_abspos()).length_sq();
			vecDrawCmdsDists.push_back({ c, flDist });
		}
	}

	if (vecDrawCmdsDists.size() > 0)
	{
		std::vector<drawcmd_t> vecDrawCmds;
		std::sort(vecDrawCmdsDists.begin(), vecDrawCmdsDists.end(), drawcmdsort());
		for (auto& dcd : vecDrawCmdsDists)
			vecDrawCmds.push_back(dcd.first);
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

void entsys::add_entity(base_entity * pEnt)
{
	m_vecEntities.push_back(pEnt);
	gpGlobals->pStatistics->get_stat_u(ESTAT_C_ENTSYS, "entity count") = m_vecEntities.size();
	
	m_vecEntityIDs.emplace(m_iNextEntityID, pEnt);
	m_iNextEntityID++;
}

void entsys::kill_entity(base_entity * pEnt)
{
	RESTRICT_THREAD_LOGIC;
	m_vecEntities.remove(pEnt);
	gpGlobals->pStatistics->get_stat_u(ESTAT_C_ENTSYS, "entity count") = m_vecEntities.size();
	delete pEnt;
	m_vecEntityIDs[m_iNextEntityID] = nullptr;
}

void entsys::get_entities(std::vector<std::pair<size_t, char[64]>>& v) const
{
	for (auto& kv : m_vecEntityIDs)
	{
		std::pair<size_t, char[64]> p;

		p.first = kv.first;
		strncpy(p.second, kv.second->get_classname(), 64);

		v.push_back(p);
	}
}

void entsys::send_updates(std::vector<entsys_update_t>& updates)
{
	m_update_buf.begin_write();
	for (auto& update : updates)
	{
		m_update_buf.write(update);
	}
	m_update_buf.end_write();
}

base_entity* entsys::get_entity(size_t iID)
{
	for (auto& kv : m_vecEntityIDs)
	{
		if (kv.first == iID)
			return kv.second;
	}
	return nullptr;
}
