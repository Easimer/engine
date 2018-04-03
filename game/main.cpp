#include "stdafx.h"
#include <enl/cmdline.h>
#include "entsys.h"
#include "igfx.h"
#include "event_handler.h"
#include <chrono>
#include "input.h"
#include "statistics.h"
#include "devgui.h"

#include <gfx/camera.h>

#include "prop_physics.h"
#include <phys/phys.h>
#include <phys/simulation.h>
#include <phys/mesh.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

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
	gpGlobals->pRenderer = new igfx();
	gpGlobals->pEventHandler = new event_handler();
	gpGlobals->pCamera = new gfx::camera();
	gpGlobals->pInput = new input();
	gpGlobals->pStatistics = new estat_container();
	//gpGlobals->pDevGUI = new devgui_state();
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
	//delete gpGlobals->pDevGUI;
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
#include <gui/imgui.h>

class window_loading : public gfx::window {
public:
	const char* get_title() override { return "engine"; }
	void draw_content() override {
		ImGui::SetWindowSize(ImVec2(200, 170));
		ImGui::Text("Loading...");
		ImGui::Text(m_str_stage.c_str());
		ImGui::ProgressBar(m_progress);
	}

	float m_progress = 0;
	std::string m_str_stage;
};

void thread_logic()
{
	gpGlobals->iThreadLogic = std::this_thread::get_id();

	std::this_thread::sleep_for(std::chrono::duration<float>(0.5));

	window_loading wnd_loading;
	gpGfx->add_window(&wnd_loading);

	gpGlobals->pRenderer->begin_load();
	wnd_loading.m_str_stage = "Compiling shaders";
	wnd_loading.m_progress = 0;
	gpGlobals->pRenderer->load_shader("data/shaders/model_dynamic.qc");
	wnd_loading.m_progress = 0.5;
	gpGlobals->pRenderer->load_shader("data/shaders/wireframe.qc");
	wnd_loading.m_progress = 1;
	//gpGlobals->pRenderer->load_shader("data/shaders/depth_map.qc"); // hardcoded into renderer

	wnd_loading.m_str_stage = "Precaching resources";
	wnd_loading.m_progress = 0;
	gpGlobals->pEntSys->precache_entities();
	wnd_loading.m_progress = 1;

	wnd_loading.m_str_stage = "Reticulating splines";
	wnd_loading.m_progress = 0;
	c_base_prop* pDog1 = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pDog1->set_model("data/models/wolf.emf");
	pDog1->spawn();
	pDog1->set_abspos(vec3(0.5, 0, -0.1));
	wnd_loading.m_progress = 0.2;
	c_base_prop* pDog2 = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pDog2->set_model("data/models/traffic_barrel.emf");
	pDog2->spawn();
	pDog2->set_abspos(vec3(-0.5, 0, 0));

	c_base_prop* pArthas = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pArthas->set_model("data/models/arthas.emf");
	pArthas->spawn();
	pArthas->set_abspos(vec3(-0.5, 0, -1));
	wnd_loading.m_progress = 0.4;
	c_base_prop* pCSoldier = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
	pCSoldier->set_model("data/models/csoldier.emf");
	pCSoldier->spawn();
	pCSoldier->set_abspos(vec3(0, 0, -0.2));

	c_base_prop* pHat = (c_base_prop*)CreateEntityNoSpawn("prop_static");
	pHat->set_model("data/models/test_terrain.smd");
	pHat->spawn();
	wnd_loading.m_progress = 0.75;
	//gpGlobals->pEntSys->kill_entity(pHat);
	
	//c_prop_physics* pSphere = (c_prop_physics*)CreateEntityNoSpawn("prop_physics");
	//if (pSphere) {
	//	pSphere->set_model("data/models/sphere.smd");
	//	pSphere->set_abspos(vec3(-10, 0, 0));
	//	/*phys::object& phys = gpGlobals->pPhysSimulation->get_object(pSphere->physics_handle());
	//	math::vector3<float> origin(-10, 0, 0);
	//	phys.position(origin);
	//	phys.velocity(math::vector3<float>(1, 0, 0));
	//	
	//	phys.collider(phys::bounding_sphere(origin, 1));*/
	//	pSphere->spawn();
	//}

	//c_prop_physics* pSphere2 = (c_prop_physics*)CreateEntityNoSpawn("prop_physics");
	//if (pSphere2) {
	//	pSphere2->set_model("data/models/sphere.smd");
	//	pSphere2->set_abspos(vec3(10, 0, 0));
	//	/*phys::object& phys = gpGlobals->pPhysSimulation->get_object(pSphere2->physics_handle());
	//	math::vector3<float> origin(10, 0, 0);
	//	phys.position(origin);
	//	phys.velocity(math::vector3<float>(-1, 0, 0));
	//	phys.collider(phys::bounding_sphere(origin, 1));*/
	//	pSphere2->spawn();
	//	
	//}
	
	wnd_loading.m_progress = 0.8;
	base_entity* pLight = CreateEntityNoSpawn("light_point");
	if (pLight) {
		pLight->spawn();
		auto pLightColor = pLight->get_keyvalue<KV_T_RGBA>("color");
		pLightColor = { 1, 1, 1, 1 };
		pLight->set_abspos(vec3(0, 10, 0));
	}
	wnd_loading.m_progress = 0.9;
	base_entity* pLightGlobal = CreateEntity("light_global");
	if (pLightGlobal) {
		pLightGlobal->set_abspos(vec3(-1.5, 0.5, 0));
		pLightGlobal->set_rotation(vec3(0, glm::radians(90.0f), glm::radians(90.0f)));
	}
	wnd_loading.m_progress = 1;
	PRINT_DBG("===========");
	PRINT_DBG("End of loading");
	PRINT_DBG("===========");
	gpGlobals->pRenderer->end_load();
	gpGfx->remove_window(&wnd_loading);

	double flNextUpdate = 0;

	while (gpGlobals->bRunning)
	{
		gpGlobals->pEventHandler->update();
		gpGlobals->pInput->update();

		gpGlobals->pPhysSimulation->simulate(gpGlobals->flDeltaTime);

		gpGlobals->pEntSys->update_entities();

		while (gpGlobals->curtime >= flNextUpdate) {
			flNextUpdate += gpGlobals->flTickTime;
			gpGlobals->pEntSys->draw_entities();
		}
		std::this_thread::sleep_for(std::chrono::duration<double>(flNextUpdate - gpGlobals->curtime));			
	}
	PRINT_DBG("Logic: joining...");
}

void thread_rendering()
{
	gpGlobals->iThreadRendering = std::this_thread::get_id();
	gpGlobals->pRenderer->init("engine", 1280, 720);

	unsigned long long nNow = SDL_GetPerformanceCounter();
	unsigned long long nLast = 0;
	 
	while (gpGlobals->bRunning)
	{
		nLast = nNow;
		nNow = SDL_GetPerformanceCounter();
		gpGlobals->flDeltaTime = ((nNow - nLast) / (double)SDL_GetPerformanceFrequency());
		gpGlobals->curtime += gpGlobals->flDeltaTime;

		gpGfx->begin_frame();

		gpGlobals->pRenderer->draw();

		gpGfx->draw_windows();
		gpGfx->end_frame();
		gpGlobals->pCamera->update(gpGlobals->flDeltaTime);
		gpGlobals->pRenderer->update();
	}

	gpGlobals->pRenderer->shutdown();
	PRINT_DBG("Renderer: joining...");
}

void thread_sound()
{
	gpGlobals->iThreadSound = std::this_thread::get_id();
}
