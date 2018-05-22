#include "stdafx.h"
#include "iclient.h"
#include "client.h"
#include <gfx/gfx.h>
#include <gfx/window_register.h>
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
#include <imgui_matrix.h>

class window_debug_entities : public gfx::window {
public:
	window_debug_entities(const std::shared_ptr<net::client> cli) : m_pClient(cli) {}
	virtual const char* get_title() { return "Entity inspector"; }
protected:
	virtual void draw_content() {
		if (m_pClient->timed_out()) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Server has timed out!");
		}
		auto edicts = m_pClient->get_edicts();
		for (size_t i = 0; i < net::max_edicts; i++) {
			if (edicts[i].active) {
				ImGui::InputInt("ID", (int*)&i, 1, 100, ImGuiInputTextFlags_ReadOnly);
				//ImGui::Checkbox("Active", &edicts[i].active);
				//ImGui::Checkbox("Updated", &edicts[i].updated);
				//ImGui::InputFloat("Last updated", &edicts[i].last_update, -1, ImGuiInputTextFlags_ReadOnly);
				//ImGui::InputFloat3("Position", (float*)edicts[i].position.ptr(), -1, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat3("iPosition", (float*)edicts[i].iposition.ptr(), -1, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat3("Velocity", (float*)edicts[i].velocity.ptr(), -1, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat4("Angular velocity", (float*)edicts[i].angular_vel, -1, ImGuiInputTextFlags_ReadOnly);
				//ImGui::InputFloat3("Acceleration", (float*)edicts[i].iacceleration.ptr(), -1, ImGuiInputTextFlags_ReadOnly);
				//ImGui::InputFloat3("iVelocity", (float*)edicts[i].ivelocity.ptr(), -1, ImGuiInputTextFlags_ReadOnly);
				ImGui::Matrix<float>("Rotation", edicts[i].rotation, 4, 4);
				ImGui::Separator();
			}
		}
	}
private:
	const std::shared_ptr<net::client> m_pClient;
};

class window_loading : public gfx::window {
public:
	window_loading() : m_flProgress(0.f), m_pszText(nullptr) {}
	virtual const char* get_title() { return "Loading..."; }

	void set_progress(float progress, const char* pszText = nullptr) {
		m_flProgress = progress;
		if (pszText)
			m_pszText = pszText;
	}
protected:
	virtual void draw_content() {
		ImGui::ProgressBar(m_flProgress, ImVec2(-1, 0), m_pszText);
	}
private:
	float m_flProgress;
	const char* m_pszText;
};

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

	std::shared_ptr<window_debug_entities> pEntDbg;
	std::shared_ptr<window_loading> pWndLoad = std::make_shared<window_loading>();

	enum client_state {
		E_CLIENT_STATE_MAINMENU = 0,
		E_CLIENT_STATE_GAME = 1,
		E_CLIENT_STATE_CONNECTING = 2,
		E_CLIENT_STATE_CONNECTING_ATTEMPT_1 = 2,
		E_CLIENT_STATE_CONNECTING_ATTEMPT_2 = 3,
		E_CLIENT_STATE_CONNECTING_ATTEMPT_3 = 4,
		E_CLIENT_STATE_CONNECTING_ATTEMPT_4 = 5,
		E_CLIENT_STATE_CONNECTING_MAX = 5,
		E_CLIENT_STATE_MAX
	} state = E_CLIENT_STATE_MAINMENU;

	std::chrono::time_point<std::chrono::high_resolution_clock> t;

	while (!cli->m_bShutdown) {
		gpGfx->begin_frame();
		gpGfx->draw_windows();

		if (state == E_CLIENT_STATE_MAINMENU) {
			mainmenu::exitcode c = mm.tick();
			if (c != mainmenu::EMMENU_OK) {
				switch (c) {
				case mainmenu::exitcode::EMMENU_START_LOCAL_GAME:
					cli->m_bRequestServer = true; // request load of server.dll
					PRINT_DBG("Loading server.dll");
				case mainmenu::exitcode::EMMENU_JOIN_REMOTE_GAME:
					gpGfx->add_window(pWndLoad);
					PRINT_DBG("Calling connect()");
					g.connect(mm.selected_server_address(), mm.selected_username());
					PRINT_DBG("Called connect()");
					std::this_thread::sleep_for(std::chrono::milliseconds(32));
					state = E_CLIENT_STATE_CONNECTING;
					PRINT_DBG("Entering CONNECTING");
					t = std::chrono::high_resolution_clock::now();
					break;
				case mainmenu::exitcode::EMMENU_QUIT_GAME:
					cli->m_bShutdown = true;
					break;
				}
			}
		}
		else if (state == E_CLIENT_STATE_GAME) {
			g.tick();
		}
		else if (state >= E_CLIENT_STATE_CONNECTING && state <= E_CLIENT_STATE_CONNECTING_MAX) {
			gpGfx->handle_events();
			if (g.get_socket()->connected()) {
				pWndLoad->set_progress(0.6, "Starting the game...");
				g.paused(false);
				if (!pEntDbg) {
					pEntDbg = std::make_shared<window_debug_entities>(g.get_socket());
					gpGfx->add_window(pEntDbg);
				}
				gpGfx->remove_window(pWndLoad);
				PRINT_DBG("Entering GAME");
				state = E_CLIENT_STATE_GAME;
			} else {
				if (state != E_CLIENT_STATE_CONNECTING_MAX) {
					auto now = std::chrono::high_resolution_clock::now();
					auto dur = now - t;
					float secs = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
					if (secs > 1.f) {
						g.get_socket()->attempt_connect();
						float prog = (float)(((int)state) - (int)E_CLIENT_STATE_CONNECTING_ATTEMPT_1) / 10;
						PRINT_DBG("Attempt: " << prog);
						(*(int*)&state)++; // HACKHACKHACK
						pWndLoad->set_progress(0.3 + prog, "Connecting to remote server...");
						t = std::chrono::high_resolution_clock::now();
					}
				} else {
					state = E_CLIENT_STATE_MAINMENU;
					mm.set_message("Failed to connect to the server!");
					gpGfx->remove_window(pWndLoad);
				}
			}
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
