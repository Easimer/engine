#include "stdafx.h"
#include <enl/cmdline.h>
#include "entsys.h"
#include "renderer.h"
#include "event_handler.h"
#include <chrono>
#include "camera.h"
#include "input.h"
#include "statistics.h"
#include "devgui.h"

#include "prop_physics.h"
#include <phys/phys.h>
#include <phys/simulation.h>
#include <phys/mesh.h>

#if defined(PLAT_LINUX)
#include <unistd.h>
#include <dirent.h>
#endif

void thread_logic();
void thread_rendering();
void thread_sound();

#undef main // SDL_main seems to crash the program at the very end, so we're doing init ourselves

/// Set the current working directory to the game root directory
/// (we're launching from /bin/)
bool set_workdir()
{
#if defined(PLAT_WINDOWS)
	DWORD ftyp = GetFileAttributesA("data");
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY && ftyp != INVALID_FILE_ATTRIBUTES) {
		PRINT_DBG("Already running from game rootdir!");
		return true;
	}

	SetCurrentDirectory("..");
	ftyp = GetFileAttributesA("data");
	if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY) || ftyp == INVALID_FILE_ATTRIBUTES) {
		PRINT_ERR("Switched to parent dir, but data/ doesn't exist!");
		return false;
	}
	return true;
#elif defined(PLAT_LINUX)
	if (chdir("..") == 0) {
		DIR* pDir = opendir("data");
		if (pDir) {
			closedir(pDir);
			return true;
		}
		else {
			PRINT_ERR("Switched to parent dir, but data/ doesn't exist!");
		}
	}
	else {
		PRINT_ERR("Couldn't switch to parent dir!");
	}
	return false;
#endif
}

int main(int argc, char** argv)
{
	CMDLINE_INIT();

	SDL_SetMainReady();
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO);

	if (!set_workdir()) {
		PRINT_ERR("Couldn't switch to rootdir!!!");
	}

	//PRINT_DBG(gpCmdline->GetExecName());
	// init globals
	ASSERT(gpGlobals);

	gpGlobals->pEntSys = new entsys();
	gpGlobals->pRenderer = new renderer();
	gpGlobals->pEventHandler = new event_handler();
	gpGlobals->pCamera = new camera();
	gpGlobals->pInput = new input();
	gpGlobals->pStatistics = new estat_container();
	gpGlobals->pDevGUI = new devgui_state();
	gpGlobals->pPhysSimulation = new phys::simulation();
	
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
	if (gpGlobals->pCommandDefs)
		delete[] gpGlobals->pCommandDefs;
	if (gpGlobals->entityFactoryDictionary)
		delete[] gpGlobals->entityFactoryDictionary;
	delete gpGlobals->pPhysSimulation;
	delete gpGlobals->pDevGUI;
	delete gpGlobals->pStatistics;
	delete gpGlobals->pInput;
	delete gpGlobals->pCamera;
	delete gpGlobals->pEventHandler;
	delete gpGlobals->pRenderer;
	delete gpGlobals->pEntSys;

	CMDLINE_SHUTDOWN();

	return 0;
}

#include "prop_common.h"

void thread_logic()
{
	gpGlobals->iThreadLogic = std::this_thread::get_id();

	std::this_thread::sleep_for(std::chrono::duration<float>(0.5));

	gpGlobals->pRenderer->begin_load();
	gpGlobals->pRenderer->load_shader("data/shaders/model_dynamic.qc");
	gpGlobals->pRenderer->load_shader("data/shaders/wireframe.qc");
	//gpGlobals->pRenderer->load_shader("data/shaders/depth_map.qc"); // hardcoded into renderer

	gpGlobals->pEntSys->precache_entities();

	/*c_base_prop* pDog1 = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pDog1->set_model("data/models/wolf.emf");
	pDog1->spawn();
	pDog1->set_abspos(vec3(0.5, 0, -0.1));

	c_base_prop* pDog2 = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pDog2->set_model("data/models/traffic_barrel.emf");
	pDog2->spawn();
	pDog2->set_abspos(vec3(-0.5, 0, 0));

	c_base_prop* pArthas = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pArthas->set_model("data/models/arthas.emf");
	pArthas->spawn();
	pArthas->set_abspos(vec3(-0.5, 0, -1));

	c_base_prop* pCSoldier = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pCSoldier->set_model("data/models/csoldier.emf");
	pCSoldier->spawn();
	pCSoldier->set_abspos(vec3(0, 0, -0.2));

	c_base_prop* pHat = (c_base_prop*)CreateEntityNoSpawn("prop_static");
	pHat->set_model("data/models/test_terrain.smd");
	pHat->spawn();
	//gpGlobals->pEntSys->kill_entity(pHat);*/

	c_prop_physics* pSphere = (c_prop_physics*)CreateEntityNoSpawn("prop_physics");
	if (pSphere) {
		pSphere->set_model("data/models/sphere.smd");
		pSphere->set_abspos(vec3(-10, 0, 0));
		/*phys::object& phys = gpGlobals->pPhysSimulation->get_object(pSphere->physics_handle());
		phys::vector3<float> origin(-10, 0, 0);
		phys.position(origin);
		phys.velocity(phys::vector3<float>(1, 0, 0));
		
		phys.collider(phys::bounding_sphere(origin, 1));*/
		pSphere->spawn();
	}

	c_prop_physics* pSphere2 = (c_prop_physics*)CreateEntityNoSpawn("prop_physics");
	if (pSphere2) {
		pSphere2->set_model("data/models/sphere.smd");
		pSphere2->set_abspos(vec3(10, 0, 0));
		/*phys::object& phys = gpGlobals->pPhysSimulation->get_object(pSphere2->physics_handle());
		phys::vector3<float> origin(10, 0, 0);
		phys.position(origin);
		phys.velocity(phys::vector3<float>(-1, 0, 0));
		phys.collider(phys::bounding_sphere(origin, 1));*/
		pSphere2->spawn();
		
	}

	base_entity* pLight = CreateEntityNoSpawn("light_point");
	if (pLight) {
		pLight->spawn();
		auto pLightColor = pLight->get_keyvalue<KV_T_RGBA>("color");
		pLightColor = { 1, 1, 1, 1 };
		pLight->set_abspos(vec3(0, 10, 0));
	}

	base_entity* pLightGlobal = CreateEntity("light_global");
	if (pLightGlobal) {
		pLightGlobal->set_abspos(vec3(-1.5, 0.5, 0));
		pLightGlobal->set_rotation(vec3(0, glm::radians(90.0f), glm::radians(90.0f)));
	}
	
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

			gpGlobals->pPhysSimulation->simulate(flSinceLastUpdate);

			gpGlobals->pEntSys->update_entities();

			// catch up
			flLastUpdate += gpGlobals->flTickTime;
			
			// sleep
			std::this_thread::sleep_for(std::chrono::duration<double>(flLastUpdate + gpGlobals->flTickTime - gpGlobals->curtime));
		}
		//if(gpGlobals->pRenderer->waiting_for_draw())
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
