#include "stdafx.h"
#include "baseentity.h"

base_entity * CreateEntity(const char * mapname)
{
	for (size_t i = 0; i < gpGlobals->iEntityFactoryDictionaryIndex; i++)
	{
		if (strncmp(gpGlobals->entityFactoryDictionary[i].szMapname, mapname, 64) == 0)
		{
			auto pEntity = gpGlobals->entityFactoryDictionary[i].pFactory->Create();
			//gpGlobals->pEntSys->add_entity(pEntity);
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
			//auto pEntity = gpGlobals->entityFactoryDictionary[i].pFactory->Create();
			//gpGlobals->pEntSys->add_entity(pEntity);
			return gpGlobals->entityFactoryDictionary[i].pFactory->Create();
		}
	}
	PRINT_ERR("Attempted to create unknown entity named" << mapname);
	return NULL;
}
