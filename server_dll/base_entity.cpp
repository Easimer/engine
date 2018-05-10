#include "stdafx.h"
#include "entity_system.h"
#include "base_entity.h"
#include "entityfactory.h"
#include <print.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

void base_entity::spawn()
{
	//m_vecPos = vec3_origin;
	//m_vecRot = vec3_origin;
	//set_parent(NULL);

	SetNextThink(DONT_THINK);

	m_bSpawned = true;

	load_keyvalues();
}

vec3 base_entity::get_abspos() const
{
	if (m_pParent)
	{
		vec3 vecAbsPos = m_pParent->get_abspos() + m_vecPos;
		return vecAbsPos;
	}
	else
	{
		return m_vecPos;
	}
}

vec3 base_entity::get_relpos() const
{
	return m_vecPos;
}

void base_entity::set_abspos(const vec3& v)
{
	if (!m_pParent)
		m_vecPos = v;
}

void base_entity::set_relpos(const vec3& v)
{
	m_vecPos = v;
}

void base_entity::set_parent(base_entity* pEnt)
{
	if (pEnt == NULL)
	{
		// detach from parent, set current (relative) position as new absolute position
		vec3 v = get_abspos();
		m_pParent = NULL;
		set_abspos(v);
	}
	else
	{
		m_pParent = pEnt;
		m_vecPos = vec3_origin;
	}
}

void base_entity::set_rotation(const vec3 & v)
{
	m_vecRot = v;
}

base_entity * CreateEntity(const char * mapname)
{
	for (size_t i = 0; i < gpGlobals->iEntityFactoryDictionaryIndex; i++)
	{
		if (strncmp(gpGlobals->entityFactoryDictionary[i].szMapname, mapname, 64) == 0)
		{
			auto pEntity = gpGlobals->entityFactoryDictionary[i].pFactory->Create();
			gpGlobals->pEntSys->add_entity(pEntity);
			pEntity->spawn();
			return pEntity;
		}
	}
	PRINT_ERR("Attempted to create unknown entity named" << mapname);
	return NULL;
}

base_entity * CreateEntityNoSpawn(const char * mapname)
{
	for (size_t i = 0; i < gpGlobals->iEntityFactoryDictionaryIndex; i++)
	{
		if (strncmp(gpGlobals->entityFactoryDictionary[i].szMapname, mapname, 64) == 0)
		{
			auto pEntity = gpGlobals->entityFactoryDictionary[i].pFactory->Create();
			gpGlobals->pEntSys->add_entity(pEntity);
			return pEntity;
		}
	}
	PRINT_ERR("Attempted to create unknown entity named" << mapname);
	return NULL;
}
