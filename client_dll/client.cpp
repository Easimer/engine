#include "stdafx.h"
#include "iclient.h"
#include "client.h"
#include <gfx/gfx.h>
#include "mainmenu.h"
#include "game.h"

extern "C" {
	ENL_EXPORT iclient* client_dll_init() {
		return new client();
	}

	ENL_EXPORT void client_dll_shutdown(iclient* p) {
		delete static_cast<client*>(p);
	}
}

void client_main(client* cli, const char* pszHostname, const char* pszUsername) {
	if (!gpGfx->init("game")) {
		PRINT_ERR("client::init: couldn't initialize graphics subsystem!!!!");
		cli->m_bShutdown = true;
		return;
	}

	mainmenu mm;
	game g;

	gpGfx->load_default_shaders();
	gpGfx->load_shader("data/shaders/model_dynamic.qc");
	//gpGfx->load_shader("data/shaders/wireframe.qc");

	while (!cli->m_bShutdown) {
		mainmenu::exitcode c;
		gpGfx->begin_frame();
		if (g.paused() && (c = mm.tick()) != mainmenu::exitcode::EMMENU_OK) {
			switch (c) {
			case mainmenu::exitcode::EMMENU_START_LOCAL_GAME:
				cli->m_bRequestServer = true;
				g.connect("::1", "LOCALUSER");
				g.paused(false);
				break;
			case mainmenu::exitcode::EMMENU_JOIN_REMOTE_GAME:
				if (cli->m_pClient) {
					cli->m_pClient.reset();
				}
				cli->m_pClient = std::make_unique<net::client>(mm.selected_server_address(), mm.selected_username());
				cli->m_pClient->connect();
				for(int i = 0; i < 3 && !cli->m_pClient->connected(); i++) {
					std::this_thread::sleep_for(std::chrono::seconds(1));
					cli->m_pClient->attempt_connect();
				}
				if (cli->m_pClient->connected())
					g.paused(false);
				else
					PRINT_ERR("Not connected, so not starting game");
				break;
			case mainmenu::exitcode::EMMENU_QUIT_GAME:
				cli->m_bShutdown = true;
				break;
			}
		} else {
			if (!g.tick())
				g.paused(true);
		}
		gpGfx->end_frame();
	}
}

void client::init(const char* pszHostname, const char* pszUsername)
{
	/*std::flush(std::cout);
	print("client::init");
	m_pclient = std::make_unique<net::client>(pszhostname, pszusername);
	for (int i = 0; i < 5; i++) {
		if (m_pclient->connected())
			break;
		std::this_thread::sleep_for(std::chrono::duration<float>(1.5f));
		m_pclient->attempt_connect();
	}
	if (!m_pclient->connected()) {
		print_err("failed to connect to listen server!");
	}*/
	m_bShutdown = false;
	m_bRequestServer = false;
	m_client_thread = std::thread(client_main, this, pszHostname, pszUsername);
}

void client::shutdown()
{
	if(m_client_thread.joinable())
		m_client_thread.join();
}

bool client::is_shutdown()
{
	return m_bShutdown;
}
