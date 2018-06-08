#include "stdafx.h"
#include "server.h"
#include "entity_system.h"
#include "prop_common.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "fps_player.h"
#include <ifsys/ifsys.h>

extern "C" {
	ENL_EXPORT iserver* server_dll_init() {
		return new server();
	}

	ENL_EXPORT void server_dll_shutdown(iserver* p) {
		delete static_cast<server*>(p);
	}

	ENL_EXPORT void ifsys_fn(ifsys* is) {
		static server ifserver;
		is->connect(ifserver.name(), &ifserver);
	}
}

void server::init() {
	std::flush(std::cout);
	
	if (m_server) return;
	
	m_server = std::make_unique<net::server>();
	
	gpGlobals->curtime = 0;
	m_server->set_time_ptr(&gpGlobals->curtime);
	m_server->set_name("Default Server Name");
	m_server->set_level_name("<no level loaded>");

	m_thread_logic = std::thread([&]() {
		std::flush(std::cout);
		PRINT_DBG("Starting logic thread");
		size_t ticks = 0;
		using timestep = std::chrono::duration<std::chrono::high_resolution_clock::rep, std::ratio<1, server_tickrate>>;
		auto next_tick = std::chrono::high_resolution_clock::now() + timestep(1);

		gpGlobals->pEntSys = new entity_system();

		// Reserve all player entities
		for (size_t i = 0; i < net::max_players; i++) {
			base_entity* pEnt = CreateEntity("player");
			auto e = m_server->get_player(i);
			e.active = false;
			e.updated = false;
		}

		// Create test entity

		c_base_prop* pDog1 = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
		pDog1->model("data/models/wolf.emf");
		pDog1->spawn();
		pDog1->set_abspos(vec3(0.5, 0, -0.1));

		c_base_prop* pDog2 = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
		pDog2->model("data/models/traffic_barrel.emf");
		pDog2->spawn();
		pDog2->set_abspos(vec3(-0.5, 0, 0));

		c_base_prop* pArthas = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
		pArthas->model("data/models/arthas.emf");
		pArthas->spawn();
		pArthas->set_abspos(vec3(-0.5, 0, -1));

		c_base_prop* pCSoldier = (c_base_prop*)CreateEntityNoSpawn("prop_dynamic");
		pCSoldier->model("data/models/csoldier.emf");
		pCSoldier->spawn();
		pCSoldier->set_abspos(vec3(0, 0, -0.2));

		c_base_prop* pHat = (c_base_prop*)CreateEntityNoSpawn("prop_static");
		pHat->model("data/models/test_terrain.smd");
		pHat->spawn();

		for (size_t i = 0; i < net::max_edicts; i++)
			m_server->edict(i).reset();

		auto f = std::bind(&server::client_input_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		m_server->set_client_input_handler(f);

		while (!m_shutdown) {			
			gpGlobals->pEntSys->update_entities();
			gpGlobals->curtime += (1.f / server_tickrate);
			
			// Update edicts
			for (base_entity* pEnt : gpGlobals->pEntSys->ptr()) {
				if (!pEnt) continue;
				if (!pEnt->networked()) continue;
				//PRINT_DBG("Updating network entity(" << pEnt->edict() << "): " << pEnt->get_classname());

				// Skip entity if it's a player, but the assigned client is offline
				if (pEnt->is_player()) {
					const net::client_desc* cd = m_server->get_client_desc(pEnt->edict() - 1);
					if (!cd->slot_active) {
						continue;
					}
				}

				net::edict_t& e = m_server->edict(pEnt->edict());
				e.active = true;
				if (e.position != pEnt->get_abspos()) {
					e.updated = true;
				}
				e.position = pEnt->get_abspos();
				//if (e.rotation2 != pEnt->get_relrot()) {
				//	e.updated = true;
				//	e.rotation2 = pEnt->get_relrot();
				//}
				if (memcmp(glm::value_ptr(pEnt->get_rotation_matrix()), e.rotation, 16 * sizeof(float)) != 0) {
					e.updated = true;
				}
				memcpy(e.rotation, glm::value_ptr(pEnt->get_rotation_matrix()), 16 * sizeof(float));
				if (pEnt->is_drawable()) {
					auto pProp = dynamic_cast<c_base_prop*>(pEnt);
					if (pProp) {
						if (strncmp(pProp->model(), e.modelname, 128) != 0) {
							e.updated = true;
							strncpy(e.modelname, pProp->model(), 128);
						}
					}
				}
				if (e.updated)
					e.last_update = gpGlobals->curtime;
			}

			m_server->push_updates();
			std::this_thread::sleep_until(next_tick);
			next_tick += timestep(1);
		}

		if(gpGlobals->pEntSys) delete gpGlobals->pEntSys;
		gpGlobals->pEntSys = nullptr;
	});
}

bool server::shutdown() {
	m_shutdown = true;
	if (m_thread_logic.joinable()) {
		m_thread_logic.join();
		return false;
	}
	if (gpGlobals->pEntSys) delete gpGlobals->pEntSys;
	return true;
}

const char * server::name() const {
	return "GameServer0001";
}

void server::client_input_handler(const net::client_desc & client, size_t edict, uint64_t cli_tick, const char * pszCommand) {
	net::edict_t& player = m_server->edict(edict);
	if (!player.active)
		return;
	base_entity* pPlayer = gpGlobals->pEntSys->get_entity_by_edict(edict);
	if (!pPlayer)
		return;
	fps_player* pFPSPlayer = (fps_player*)pPlayer;
	vec3 pos = pFPSPlayer->get_abspos();
	vec3& vel = pFPSPlayer->velocity();
	if (strcmp(pszCommand, "+forward") == 0)
		vel[2] += 1;
	if (strcmp(pszCommand, "-forward") == 0)
		vel[2] -= 1;
	if (strcmp(pszCommand, "+backward") == 0)
		vel[2] -= 1;
	if (strcmp(pszCommand, "-backward") == 0)
		vel[2] += 1;
	if (strcmp(pszCommand, "+moveleft") == 0)
		vel[0] -= 1;
	if (strcmp(pszCommand, "-moveleft") == 0)
		vel[0] += 1;
	if (strcmp(pszCommand, "+moveright") == 0)
		vel[0] += 1;
	if (strcmp(pszCommand, "-moveright") == 0)
		vel[0] -= 1;
}
