#pragma once

#include "entityfactory.h"

struct entfmap_t;
class entsys;
class renderer;

struct engine_globals {
	entfmap_t* entityFactoryDictionary;
	size_t iEntityFactoryDictionaryIndex;
	entsys* pEntSys;
	renderer* pRenderer;
};

extern engine_globals* gpGlobals;
