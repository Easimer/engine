#pragma once

#include "entityfactory.h"

struct entfmap_t;
class entsys;
class renderer;

struct engine_globals {
	// Entity Factory
	entfmap_t* entityFactoryDictionary;
	size_t iEntityFactoryDictionaryIndex;
	
	// Time
	float curtime = 0;
	float flDeltaTime = 0;

	// Game
	bool bRunning = true;

	// Entity System
	entsys* pEntSys;
	// Renderer
	renderer* pRenderer;
	// Thread IDs
	std::thread::id iThreadLogic;
	std::thread::id iThreadRendering;
	std::thread::id iThreadSound;
};

extern engine_globals* gpGlobals;

// Triggers an assert if the current thread's ID doesn't equal 'tid'
#define RESTRICT_THREAD(iThreadId) ASSERT((std::this_thread::get_id()) == (iThreadId));
#define RESTRICT_THREAD_LOGIC RESTRICT_THREAD(gpGlobals->iThreadLogic)
#define RESTRICT_THREAD_RENDERING RESTRICT_THREAD(gpGlobals->iThreadRendering)
#define RESTRICT_THREAD_SOUND RESTRICT_THREAD(gpGlobals->iThreadSound)
