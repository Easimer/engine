#include "stdafx.h"
#include "server_list.h"
#include <gui/imgui.h>

inline bool operator==(const sockaddr_in6& lhs, const sockaddr_in6& rhs) {
	bool ports = lhs.sin6_port == rhs.sin6_port;
	for (size_t i = 0; i < 16; i++) {
		if (lhs.sin6_addr.u.Byte[i] != rhs.sin6_addr.u.Byte[i])
			return false;
	}
	return ports;
}

server_list::server_list() : m_pSelected(nullptr) {}

void server_list::add_server(const sockaddr_in6 & srv) {
	// Do we have this already
	for (auto& s : m_servers) {
		if (s == srv)
			return;
	}
	m_servers.push_back(srv);
}

void server_list::render() {
	char buf[128];
	if (ImGui::Begin("Server Browser")) {
		ImGui::ListBoxHeader("Servers");
		for (auto& srv : m_servers) {
			inet_ntop(AF_INET6, &srv.sin6_addr, buf, 128);
			strncat(buf, std::to_string(srv.sin6_port).c_str(), 128);
			if (ImGui::Selectable(buf, (&srv) == m_pSelected)) {
				m_pSelected = &srv;
			}
		}
		ImGui::ListBoxFooter();
	}

	ImGui::End();
}
