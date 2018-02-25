#pragma once

#include "entityfactory.h"
#include "statistics.h"

struct entfmap_t;
class entsys;
class renderer;
class event_handler;
class camera;
class input;

struct engine_globals {
	// Entity Factory
	entfmap_t* entityFactoryDictionary;
	size_t iEntityFactoryDictionaryIndex;
	
	// Time
	double curtime = 0;
	double flDeltaTime = 0;

	// Game
	bool bRunning = true;
	event_handler* pEventHandler;
	camera* pCamera;
	input* pInput;

	// Entity System
	entsys* pEntSys;
	// Renderer
	renderer* pRenderer;
	// Thread IDs
	std::thread::id iThreadLogic;
	std::thread::id iThreadRendering;
	std::thread::id iThreadSound;

	// Engine Statistics
	estat_container* pStatistics;

	// TEMP
	float flRot_ = 0;
};

extern engine_globals* gpGlobals;

// Triggers an assert if the current thread's ID doesn't equal 'tid'
#define RESTRICT_THREAD(iThreadId) ASSERT((std::this_thread::get_id()) == (iThreadId));
#define RESTRICT_THREAD_LOGIC RESTRICT_THREAD(gpGlobals->iThreadLogic)
#define RESTRICT_THREAD_RENDERING RESTRICT_THREAD(gpGlobals->iThreadRendering)
#define RESTRICT_THREAD_SOUND RESTRICT_THREAD(gpGlobals->iThreadSound)

typedef size_t model_id;
