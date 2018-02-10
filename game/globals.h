#pragma once

#include "entityfactory.h"

struct entfmap_t;
class entsys;

struct engine_globals {
	entfmap_t* entityFactoryDictionary;
	size_t iEntityFactoryDictionaryIndex;
	entsys* pEntSys;
};

extern engine_globals* gpGlobals;
