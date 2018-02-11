#pragma once
#include "stdafx.h"
#include "entsys.h"

entsys::entsys()
{
	m_pPages = new entsys_page[ESPAGE_COUNT];
	ASSERT(m_pPages);
	m_pFreePages = new entsys_page*[ESPAGE_COUNT];
	ASSERT(m_pFreePages);
	for (size_t i = 0; i < ESPAGE_COUNT; i++)
	{
		m_pFreePages[i] = m_pPages + i;
		base_entity* pEnt = (base_entity*)m_pFreePages[i];
		pEnt->set_eid(i);
	}
}

entsys::~entsys()
{
	if (m_pPages)
		delete[] m_pPages;
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

void* entsys::request_page()
{
	for (size_t i = 0; i < ESPAGE_COUNT; i++)
	{
		if (m_pFreePages[i])
		{
			void* pPage = m_pFreePages[i];
			m_pFreePages[i] = NULL;
			return pPage;
		}
	}
	PRINT_DBG("entsys: OUT OF PAGES!!");
	return NULL;
}

void entsys::free_page(void * pPage)
{
	ASSERT(pPage);
	if (!pPage)
		return;
	if (pPage < m_pPages || pPage >= (m_pPages + ESPAGE_COUNT))
	{
		ASSERT(0);
		return;
	}
	/*for (size_t i = 0; i < ESPAGE_COUNT; i++)
	{
		if (!m_pFreePages[i])
		{
			m_pFreePages[i] = (entsys_page*)pPage;
		}
	}*/

	// remove from active entity list
	for (auto pEnt : m_vecEntities)
	{
		if (pEnt == pPage)
			m_vecEntities.remove(pEnt);
	}

	base_entity* pEnt = (base_entity*)pPage;
	m_pFreePages[pEnt->m_iEId] = (entsys_page*)pPage;
}
