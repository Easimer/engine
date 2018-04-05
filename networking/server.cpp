#include "stdafx.h"
#include <net/server.h>
#include <thread>

net::server::server() {
	socket_t s;
	struct sockaddr_in6 server;
	int slen;
	WSADATA wsa;

	slen = sizeof(server);
#if defined(PLAT_WINDOWS)
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		PRINT_ERR("net::server::ctor: can't init WS: " << WSAGetLastError());
		return;
	}
#endif

	if ((s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == invalid_socket) {
#if defined(PLAT_WINDOWS)
		PRINT_ERR("net::server::ctor: can't open socket: " << WSAGetLastError());
#elif defined(PLAT_LINUX)
		PRINT_ERR("net::server::ctor: can't open socket: " << strerror());
#endif
		return;
	}
#if !defined(PLAT_WINDOWS_XP) // TODO: define this macro
	int zero = 0;
	setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&zero, sizeof(zero));
#endif

	memset(&server, 0, sizeof(server));
	server.sin6_family = AF_INET6;
	server.sin6_addr = in6addr_any;
	//inet_pton(AF_INET6, "::", (void*)&server.sin6_addr.s6_addr);
	server.sin6_port = htons(net::port);
	server.sin6_scope_id = 0;

	if (bind(s, (sockaddr*)&server, slen) == net::socket_error) {
#if defined(PLAT_WINDOWS)
		PRINT_ERR("net::server::ctor: can't bind socket: " << WSAGetLastError());
		closesocket(s);
#elif defined(PLAT_LINUX)
		PRINT_ERR("net::server::ctor: can't bind socket: " << strerror());
		close(s)
#endif
		return;
	}

	m_socket = s;
	m_listening = true;

	add_handles(); // server_handles.cpp

	std::thread t([&]() {
		auto socket = get_socket();
		PRINT_DBG("net::server: receiver thread is running");
		while (true) {
			char buf[4096];
			int recv_len;
			sockaddr_in6 from;
			int slen = sizeof(from);

			if ((recv_len = recvfrom(socket, buf, 4096, 0, (sockaddr*)&from, &slen)) != net::socket_error) {
				char addrbuf[64];
				inet_ntop(AF_INET6, &from.sin6_addr, addrbuf, 64);
				PRINT_DBG("net::server::thread: received " << recv_len << " bytes from " << addrbuf);
				auto verifier = flatbuffers::Verifier((const uint8_t*)buf, recv_len);
				if (Schemas::Networking::VerifyMessageHeaderBuffer(verifier)) {
					auto msghdr = Schemas::Networking::GetMessageHeader(buf);
					ASSERT(msghdr->Verify(verifier));

					auto msgtype = (*msghdr).type();

					if (m_handlers.count(msgtype)) {
						m_handlers[msgtype](from, *msghdr, recv_len);
					} else {
						PRINT_DBG("net::server::thread: unhandleable message type: " << Schemas::Networking::EnumNameMessageType(msgtype));
					}
				} else {
					PRINT_ERR("net::server: verify failed!");
				}
			}
		}
	});
	t.detach();
}

net::server::~server() {
	if (m_listening) {
		closesocket(m_socket);
	}
}

void net::server::on_connect() {

}

void net::server::update_state(const ent_id id, const edict_t & e) {
	edict_t& ed = m_edicts[id];
	if (!ed.active && e.active) {
		ed.active = true;
	}
	if (ed.active) {
		memcpy(&ed, &e, sizeof(edict_t));
		ed.updated = true;
	}
}

void net::server::broadcast_update() {
}

void net::server::send_to_client(const sockaddr_in6 & client, const void * pBuf, size_t nSiz) {
	size_t nSent = 0;
	if ((nSent = sendto(m_socket, (char*)pBuf, nSiz, 0, (sockaddr*)&client, sizeof(client))) == net::socket_error) {
#if defined(PLAT_WINDOWS)
		auto iErr = WSAGetLastError();
		char buf[256];
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, iErr, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buf, 256, NULL);
		PRINT_ERR("net::server::sendtoclient: " << buf);
#endif
	}
	ASSERT(nSent == nSiz);
}

net::client_desc * net::server::get_client_desc(const sockaddr_in6& addr) {
	for (size_t i = 0; i < get_max_players(); i++) {
		net::client_desc& c = m_clients[i];
		if (!c.slot_active)
			continue;
		if (c.addr == addr)
			return &c;
	}
	return nullptr;
}

net::client_desc * net::server::get_client_desc(const std::string_view& username) {
	for (size_t i = 0; i < get_max_players(); i++) {
		net::client_desc& c = m_clients[i];
		if (!c.slot_active)
			continue;
		if (c.username == username)
			return &c;
	}
	return nullptr;
}
