#include "stdafx.h"
#include "mainmenu.h"
#include <gui/imgui.h>
#include <gfx/gfx.h>

inline bool operator==(const sockaddr_in6& lhs, const sockaddr_in6& rhs) {
	bool ports = lhs.sin6_port == rhs.sin6_port;
	for (size_t i = 0; i < 16; i++) {
		if (lhs.sin6_addr.u.Byte[i] != rhs.sin6_addr.u.Byte[i])
			return false;
	}
	return ports;
}

mainmenu::mainmenu() : m_pSelected(nullptr), m_pMsg(nullptr) {
	m_szAddressBuf[0] = 0;
	m_szUsername[0] = 0;
}

mainmenu::exitcode mainmenu::tick() {
	mainmenu::exitcode ret = EMMENU_OK;
	char buf[128];

	gpGfx->handle_events();

	if (ImGui::Begin("")) {

		if (ImGui::Button("Join Game")) {
			//ret = EMMENU_JOIN_REMOTE_GAME;
			m_bShowServerBrowser = true;
			if (!m_pSDClient) {
				m_pSDClient = std::make_shared<net::server_discovery>();
			}
			discover_servers();
		}
		if (ImGui::Button("Host Game")) {
			ret = EMMENU_START_LOCAL_GAME;
			strncpy(m_szAddressBuf, "127.0.0.1", 128);
			strncpy(m_szUsername, "LOCALUSER", 128);
		}
		if (ImGui::Button("Quit")) {
			ret = EMMENU_QUIT_GAME;
		}
	}
	ImGui::End();

	if (m_bShowServerBrowser) {
		if (ImGui::Begin("Server Browser")) {
			ImGui::ListBoxHeader("Servers");
			for (auto& srv : m_servers) {
				buf[0] = 0;
				strncat(buf, srv.name.c_str(), 128);
				strncat(buf, " - ", 128);
				if (ImGui::Selectable(buf, &srv == m_pSelected)) {
					if (m_pSelected != &srv) {
						inet_ntop(AF_INET, &srv.addr.sin_addr, buf, 128);
						strncpy(m_szAddressBuf, buf, 128);
					}
					m_pSelected = &srv;
				}
			}
			ImGui::ListBoxFooter();
			if (m_pSelected) {
				inet_ntop(AF_INET, &m_pSelected->addr.sin_addr, buf, 128);
				strncat(buf, ":", 128);
				strncat(buf, std::to_string(ntohs(m_pSelected->addr.sin_port)).c_str(), 128);
				ImGui::InputText("Address", buf, 128, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputText("Level", (char*)m_pSelected->level.c_str(), m_pSelected->level.size(), ImGuiInputTextFlags_ReadOnly);
				snprintf(buf, 128, "%llu/%llu", m_pSelected->players, m_pSelected->max_players);
				ImGui::InputText("Players", buf, 128, ImGuiInputTextFlags_ReadOnly);
				ImGui::Separator();
			}
			ImGui::InputText("Address", m_szAddressBuf, 128);
			ImGui::NewLine();
			ImGui::InputText("Player name", m_szUsername, 128);
			ImGui::NewLine();
			if (ImGui::Button("Refresh")) {
				discover_servers();
			}
			ImGui::SameLine();
			if (ImGui::Button("Connect")) {
				ret = EMMENU_JOIN_REMOTE_GAME;
			}
			if (m_pMsg) {
				ImGui::NewLine();
				ImGui::TextColored(ImVec4(1, 0, 0, 1), m_pMsg);
			}
		}
		ImGui::End();
	}

	return ret;
}

size_t mainmenu::discover_servers() {
	m_pSDClient->probe();
	m_pSDClient->timeout(1);
	m_pSDClient->fetch();
	size_t ret = 0;
	for (const auto& srv : m_pSDClient->get()) {
		for (auto& my_srv : m_servers)
			if (srv == my_srv)
				continue;
		m_servers.push_back(srv);
		ret++;
	}
	return ret;
}
