#include "stdafx.h"
#include <enl/cmdline.h>
#include "entsys.h"
#include "renderer.h"
#include "event_handler.h"
#include <chrono>
#include "camera.h"
#include "input.h"
#include "statistics.h"
#include "memstat.h"

void thread_logic();
void thread_rendering();
void thread_sound();

#undef main // SDL_main seems to crash the program at the very end, so we're doing init ourselves

int main(int argc, char** argv)
{
	memstat_init();
	memstat_enabled = true;
	CMDLINE_INIT();

	SDL_SetMainReady();
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO);

	//PRINT_DBG(gpCmdline->GetExecName());
	// init globals
	ASSERT(gpGlobals);

	gpGlobals->pEntSys = new entsys();
	gpGlobals->pRenderer = new renderer();
	gpGlobals->pEventHandler = new event_handler();
	gpGlobals->pCamera = new camera();
	gpGlobals->pInput = new input();
	gpGlobals->pStatistics = new estat_container();
	
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

	PRINT_DBG("Main: threads joined!");

	// deinit globals
	delete gpGlobals->pStatistics;
	delete gpGlobals->pInput;
	delete gpGlobals->pCamera;
	delete gpGlobals->pEventHandler;
	delete gpGlobals->pRenderer;
	delete gpGlobals->pEntSys;

	memstat_enabled = false;
	memstat_print();

	//CMDLINE_SHUTDOWN(); // CRASH HERE

	SDL_Quit();

	return 0;
}

#include "prop_common.h"

void thread_logic()
{
	gpGlobals->iThreadLogic = std::this_thread::get_id();

	std::this_thread::sleep_for(std::chrono::duration<float>(0.5));

	gpGlobals->pRenderer->begin_load();
	gpGlobals->pRenderer->load_shader("data/shaders/model_dynamic.qc");

	gpGlobals->pEntSys->precache_entities();

	c_base_prop* pWolf = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pWolf->set_model("data/models/wolf.smd");
	pWolf->spawn();
	pWolf->set_relpos(vec3(0, -0.5, 0));
	
	PRINT_DBG("===========");
	PRINT_DBG("End of loading");
	PRINT_DBG("===========");
	gpGlobals->pRenderer->end_load();

	double flLastUpdate = 0;

	while (gpGlobals->bRunning)
	{
		bool bEarly = false;
		double flSinceLastUpdate = gpGlobals->curtime - flLastUpdate;
		double flNextUpdate = gpGlobals->curtime + gpGlobals->flTickTime;
		// Only update 64 times a sec
		if (flSinceLastUpdate >= gpGlobals->flTickTime)
		{
			gpGlobals->pEventHandler->update();
			gpGlobals->pInput->update();

			gpGlobals->pEntSys->update_entities();

			// catch up
			flLastUpdate += gpGlobals->flTickTime;
			
			// sleep
			std::this_thread::sleep_for(std::chrono::duration<double>(flLastUpdate + gpGlobals->flTickTime - gpGlobals->curtime));
		}
		if(gpGlobals->pRenderer->waiting_for_draw())
			gpGlobals->pEntSys->draw_entities();
	}
	PRINT_DBG("Logic: joining...");
}

void thread_rendering()
{
	gpGlobals->iThreadRendering = std::this_thread::get_id();
	gpGlobals->pRenderer->open_window("engine", 1280, 720, false);
	gpGlobals->pRenderer->init_gl();
	gpGlobals->pRenderer->init_gui();

	gpGlobals->pRenderer->load_loop();

	unsigned long long nNow = SDL_GetPerformanceCounter();
	unsigned long long nLast = 0;

	while (gpGlobals->bRunning)
	{
		nLast = nNow;
		nNow = SDL_GetPerformanceCounter();
		gpGlobals->flDeltaTime = ((nNow - nLast) / (double)SDL_GetPerformanceFrequency());
		gpGlobals->curtime += gpGlobals->flDeltaTime;

		gpGlobals->pRenderer->render();
		gpGlobals->pCamera->update();
	}

	gpGlobals->pRenderer->shutdown_gui();
	PRINT_DBG("Renderer: shutting down GL");
	gpGlobals->pRenderer->shutdown_gl();
	PRINT_DBG("Renderer: shutting down SDL2");
	gpGlobals->pRenderer->close_window();
	PRINT_DBG("Renderer: joining...");
}

void thread_sound()
{
	gpGlobals->iThreadSound = std::this_thread::get_id();
}
