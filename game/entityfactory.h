#pragma once

#include "stdafx.h"
#include "baseentity.h"

class ientity_factory {
public:
	virtual base_entity* Create() = 0;
};

struct entfmap_t {
	char szMapname[64];
	ientity_factory* pFactory;
};

template<class T>
class entity_factory : public ientity_factory {
public:
	entity_factory(const char* szName)
	{
		if (gpGlobals->entityFactoryDictionary == NULL)
			gpGlobals->entityFactoryDictionary = new entfmap_t[128];
		strncpy(gpGlobals->entityFactoryDictionary[gpGlobals->iEntityFactoryDictionaryIndex].szMapname, szName, 64);
		gpGlobals->entityFactoryDictionary[gpGlobals->iEntityFactoryDictionaryIndex].pFactory = this;
		gpGlobals->iEntityFactoryDictionaryIndex++;
	}

	virtual base_entity* Create()
	{
		return new T();
	}
};
