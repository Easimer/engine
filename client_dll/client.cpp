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

#include <gfx/window.h>
#include <gfx/window_register.h>
#include <gui/imgui.h>

class window_debug_entities : public gfx::window {
public:
	window_debug_entities(const net::client* cli) : m_pClient(cli) {}
	virtual const char* get_title() { return "Entity inspector"; }
protected:
	virtual void draw_content() {
		auto edicts = m_pClient->get_edicts();
		for (size_t i = 0; i < net::max_edicts; i++) {
			if (edicts[i].active) {
				ImGui::InputInt("ID", (int*)&i, 1, 100, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat3("Position", (float*)edicts[i].position.ptr(), -1, ImGuiInputTextFlags_ReadOnly);
			}
		}
	}
private:
	const net::client* m_pClient;
};

void client_main(client* cli, const char* pszHostname, const char* pszUsername) {
	if (!gpGfx->init("game")) {
		PRINT_ERR("client::init: couldn't initialize graphics subsystem!!!!");
		cli->m_bShutdown = true;
		return;
	}

	mainmenu mm;
	game g;

	std::unique_ptr<window_debug_entities> wnd_de;

	gpGfx->load_default_shaders();
	gpGfx->load_shader("data/shaders/model_dynamic.qc");
	//gpGfx->load_shader("data/shaders/wireframe.qc");

	while (!cli->m_bShutdown) {
		mainmenu::exitcode c;
		gpGfx->begin_frame();
		gpGfx->draw_windows();
		if (g.paused()) {
			if ((c = mm.tick()) != mainmenu::exitcode::EMMENU_OK) {
				switch (c) {
				case mainmenu::exitcode::EMMENU_START_LOCAL_GAME:
					cli->m_bRequestServer = true;
					g.connect("::1", "LOCALUSER");
					g.paused(false);
					wnd_de = std::make_unique<window_debug_entities>(g.get_socket());
					gpGfx->add_window(wnd_de.get());
					break;
				case mainmenu::exitcode::EMMENU_JOIN_REMOTE_GAME:
					g.connect(mm.selected_server_address(), mm.selected_username());
					for (int i = 0; i < 3 && !g.get_socket()->connected(); i++) {
						std::this_thread::sleep_for(std::chrono::seconds(1));
						g.get_socket()->attempt_connect();
					}
					
					if (g.get_socket()->connected()) {
						g.paused(false);
						wnd_de = std::make_unique<window_debug_entities>(g.get_socket());
						gpGfx->add_window(wnd_de.get());
					} else
						PRINT_ERR("Not connected, so not starting game");
					break;
				case mainmenu::exitcode::EMMENU_QUIT_GAME:
					cli->m_bShutdown = true;
					if (wnd_de)
						wnd_de.reset();
					break;
				}
			}
		} else {
			/*if (!g.tick())
				g.paused(true);*/
			g.tick();
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
