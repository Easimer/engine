#include "stdafx.h"
#include <enl/cmdline.h>
#include "entsys.h"
#include "renderer.h"
#include "memstat.h"

void thread_logic();
void thread_rendering();
void thread_sound();

int main(int argc, char** argv)
{
	memstat_init();
	memstat_enabled = true;
	CMDLINE_INIT();

	//PRINT_DBG(gpCmdline->GetExecName());
	// init globals
	ASSERT(gpGlobals);

	gpGlobals->pEntSys = new entsys();
	gpGlobals->pRenderer = new renderer();
	
	// Start main threads

	std::thread thread_logic_(thread_logic);
	PRINT_DBG("Logic thread started");
	std::thread thread_rendering_(thread_rendering);
	PRINT_DBG("Rendering thread started");
	std::thread thread_sound_(thread_sound);
	PRINT_DBG("Sound thread started");

	thread_rendering_.join();
	thread_logic_.join();
	thread_sound_.join();

	// deinit globals
	delete gpGlobals->pRenderer;
	delete gpGlobals->pEntSys;

	memstat_enabled = false;
	memstat_print();

#if defined(PLAT_DEBUG)

#endif

#if defined(PLAT_DEBUG) // wait for key
	PRINT("Press a key to exit...");
	std::string a;
	std::cin >> a;
#endif

	//CMDLINE_SHUTDOWN(); // CRASH HERE

	return 0;
}

void thread_logic()
{
	gpGlobals->iThreadLogic = std::this_thread::get_id();

	gpGlobals->pRenderer->begin_load();
	gpGlobals->pRenderer->load_model("../data/models/cowboy_hat.smd");
	gpGlobals->pRenderer->end_load();
}

void thread_rendering()
{
	gpGlobals->iThreadRendering = std::this_thread::get_id();
	gpGlobals->pRenderer->open_window("engine", 1280, 720, false);
	gpGlobals->pRenderer->init_gl();

	gpGlobals->pRenderer->model_load_loop();

	gpGlobals->pRenderer->shutdown_gl();
	gpGlobals->pRenderer->close_window();
}

void thread_sound()
{
	gpGlobals->iThreadSound = std::this_thread::get_id();
}
