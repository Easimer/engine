#include "stdafx.h"
#include "server.h"
#include "entity_system.h"

extern "C" {
	ENL_EXPORT iserver* server_dll_init() {
		return new server();
	}

	ENL_EXPORT void server_dll_shutdown(iserver* p) {
		delete static_cast<server*>(p);
	}
}

void server::init() {
	PRINT("server::init");
	
	gpGlobals->curtime = 0;

	auto time_start = std::chrono::high_resolution_clock::now();

	m_thread_logic = std::thread([&]() {
		float next_tick = 0;
		while (!m_shutdown) {
			if (next_tick < gpGlobals->curtime) {
				gpGlobals->pEntSys->update_entities();
			}
			auto time_now = std::chrono::high_resolution_clock::now();
			gpGlobals->curtime = std::chrono::duration_cast<std::chrono::seconds>(time_now - time_start).count();
			next_tick = gpGlobals->curtime + server_tickrate;

			std::this_thread::sleep_for(std::chrono::duration<float>(next_tick));
		}
	});
}

void server::shutdown() {
	m_shutdown = true;
	m_thread_logic.join();
}
