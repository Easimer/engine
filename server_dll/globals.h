#pragma once

struct entfmap_t;
class entity_system;

struct globals {
	entfmap_t* entityFactoryDictionary = nullptr;
	size_t iEntityFactoryDictionaryIndex = 0;
	entity_system* pEntSys;
	float curtime;
};

extern globals* gpGlobals;

