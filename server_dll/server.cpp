#include "stdafx.h"
#include "server.h"
#include "entity_system.h"
#include "prop_common.h"

extern "C" {
	ENL_EXPORT iserver* server_dll_init() {
		return new server();
	}

	ENL_EXPORT void server_dll_shutdown(iserver* p) {
		delete static_cast<server*>(p);
	}
}

void server::init() {
	std::flush(std::cout);
	PRINT("server::init");
	
	gpGlobals->curtime = 0;

	m_thread_logic = std::thread([&]() {
		std::flush(std::cout);
		PRINT_DBG("Starting logic thread");
		size_t ticks = 0;
		using timestep = std::chrono::duration<std::chrono::high_resolution_clock::rep, std::ratio<1, server_tickrate>>;
		auto next_tick = std::chrono::high_resolution_clock::now() + timestep(1);

		gpGlobals->pEntSys = new entity_system();

		// Create test entity

		c_base_prop* pEnt = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
		pEnt->model("data/models/wolf.emf");
		pEnt->spawn();

		c_base_prop* pEnt2 = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");

		while (!m_shutdown) {
			gpGlobals->pEntSys->update_entities();
			gpGlobals->curtime += (1.f / server_tickrate);
			
			// Update edicts
			for (base_entity* pEnt : gpGlobals->pEntSys->ptr()) {
				if (!pEnt) continue;
				if (!pEnt->networked()) continue;
				//PRINT_DBG("Updating network entity(" << pEnt->edict() << "): " << pEnt->get_classname());
				net::edict_t& e = m_server.edict(pEnt->edict());
				e.active = true;
				if (e.position != pEnt->get_abspos()) {
					e.updated = true;
					e.position = pEnt->get_abspos();
				}
				if (e.rotation2 != pEnt->get_relrot()) {
					e.updated = true;
					e.rotation2 = pEnt->get_relrot();
				}
				if (pEnt->is_drawable()) {
					auto pProp = dynamic_cast<c_base_prop*>(pEnt);
					if (pProp) {
						if (strncmp(pProp->model(), e.modelname, 128) != 0) {
							e.updated = true;
							strncpy(e.modelname, pProp->model(), 128);
						}
					}
				}
			}

			m_server.push_updates();
			std::this_thread::sleep_until(next_tick);
			next_tick += timestep(1);
		}

		if(gpGlobals->pEntSys) delete gpGlobals->pEntSys;
		gpGlobals->pEntSys = nullptr;
	});
}

void server::shutdown() {
	if(m_thread_logic.joinable())
		m_thread_logic.join();
	if (gpGlobals->pEntSys) delete gpGlobals->pEntSys;
}
