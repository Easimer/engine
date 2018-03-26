#pragma once

//#include "entityfactory.h"
#include "statistics.h"

struct entfmap_t;
class entsys;
class renderer;
class event_handler;
class icamera;
class input;
struct devgui_state;
struct ccommand_def;

namespace phys {
	class simulation;
}

struct engine_globals {
	/// Engine Core
	// Renderer
	renderer* pRenderer;
	// Window event handler
	event_handler* pEventHandler;
	// Camera
	icamera* pCamera;
	// input handler
	input* pInput;
	// Engine Statistics
	estat_container* pStatistics;
	// Set this to false and the engine will shut down.
	bool bRunning = true;
	// Whether main menu is open. Open by default.
	bool bPaused = true;
	// Is the DevGUI open? If so, events will only be passed to imgui.
	bool bDevGUI = false;
	// Thread IDs
	std::thread::id iThreadLogic;
	std::thread::id iThreadRendering;
	std::thread::id iThreadSound;
	// DevGUI State
	devgui_state* pDevGUI;

	/// Time
	// Current time in seconds / time elapsed since engine init
	double curtime = 0;
	// Time elapsed since last frame
	double flDeltaTime = 0;
	// Approx. time elapsed since last logic update
	// Used for timimg of the logic thread loop
	const double flTickTime = (1.0 / 64.0);

	/// Entity System
	entsys* pEntSys;
	// Entity Factory
	entfmap_t* entityFactoryDictionary = nullptr;
	size_t iEntityFactoryDictionaryIndex = 0;

	/// Console
	ccommand_def* pCommandDefs = nullptr;
	size_t iCommandDefs = 0;
	bool bConsoleOpen = false;

	/// Physics simulation
	phys::simulation* pPhysSimulation = nullptr;
};

extern engine_globals* gpGlobals;

// Triggers an assert if the current thread's ID doesn't equal 'tid'
#define RESTRICT_THREAD(iThreadId) ASSERT((std::this_thread::get_id()) == (iThreadId));
#define RESTRICT_THREAD_LOGIC RESTRICT_THREAD(gpGlobals->iThreadLogic)
#define RESTRICT_THREAD_RENDERING RESTRICT_THREAD(gpGlobals->iThreadRendering)
#define RESTRICT_THREAD_SOUND RESTRICT_THREAD(gpGlobals->iThreadSound)

typedef size_t model_id;
