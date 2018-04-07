#include "stdafx.h"
#include <thread>
#include <net/client.h>

net::client::client(const std::string& addr, const std::string& username) : m_username(username) {
	socket_t s;
	
	int slen;
	WSADATA wsa;

	slen = sizeof(m_server_addr);
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

	m_socket = s;

	memset(&m_server_addr, 0, sizeof(m_server_addr));
	m_server_addr.sin6_family = AF_INET6;
	inet_pton(AF_INET6, addr.c_str(), &m_server_addr.sin6_addr.s6_addr);
	m_server_addr.sin6_port = htons(net::port);
	m_server_addr_siz = sizeof(m_server_addr);

	m_connected = false;
	connect();
}

net::client::~client() {
	disconnect();
}

void net::client::send_update(const net::client_update & update) {
}

void net::client::send_to_server(const void * pBuf, size_t nSiz) {
	size_t sent = 0;
	if ((sent = sendto(m_socket, (char*)pBuf, nSiz, 0, (sockaddr*)&m_server_addr, m_server_addr_siz)) == net::socket_error) {
#if defined(PLAT_WINDOWS)
		auto iErr = WSAGetLastError();
		char buf[256];
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, iErr, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buf, 256, NULL);
		PRINT_ERR("net::server::sendtoclient: " << buf);
#endif
	}
	ASSERT(nSiz == sent);
}

void net::client::add_handlers() {
	
}

void net::client::attempt_connect() {
	// Build connect packet
	flatbuffers::FlatBufferBuilder fbb;

	auto username_off = fbb.CreateString(m_username);
	Schemas::Networking::ConnectDataBuilder cdb(fbb);
	cdb.add_username(username_off);
	auto cdb_off = cdb.Finish();

	Schemas::Networking::MessageHeaderBuilder mhb(fbb);
	mhb.add_type(Schemas::Networking::MessageType::MessageType_CONNECT);
	mhb.add_data(cdb_off.Union());
	mhb.add_data_type(Schemas::Networking::MessageData::MessageData_ConnectData);
	Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());

	size_t siz_expected = fbb.GetSize();
	int siz_sent = 0;

	if ((siz_sent = sendto(m_socket, (char*)fbb.GetBufferPointer(), siz_expected, 0, (sockaddr*)&m_server_addr, sizeof(m_server_addr))) == net::socket_error) {
		PRINT_ERR("net::client::connect: ERR: " << WSAGetLastError());
		ASSERT(0);
	}
	PRINT_DBG(siz_sent);
	ASSERT(siz_sent == siz_expected);
	PRINT_DBG("net::client::connect: sent connection request");
}

void net::client::connect() {
	if (m_connected)
		return;
	m_connected = false;

	std::thread t([&]() {
		auto socket = get_socket();
		PRINT_DBG("net::client: receiver thread is running");
		while (true) {
			char buf[4096];
			int recv_len;
			sockaddr_in6 from;
			int slen = sizeof(from);

			if ((recv_len = recvfrom(socket, buf, 4096, 0, (sockaddr*)&from, &slen)) != net::socket_error) {
				PRINT_DBG("net::client::thread: received " << recv_len << " bytes");
				auto verifier = flatbuffers::Verifier((const uint8_t*)buf, recv_len);
				if (Schemas::Networking::VerifyMessageHeaderBuffer(verifier)) {
					auto msghdr = Schemas::Networking::GetMessageHeader(buf);
					ASSERT(msghdr->Verify(verifier));
					switch ((*msghdr).type()) {
					case Schemas::Networking::MessageType_NONE:
						PRINT_ERR("net::client::thread: keepalive from server");
						break;
					case Schemas::Networking::MessageType_CONNECT_ACK:
						PRINT_ERR("net::client::thread: server acknowledges connection!");
						m_connected = true;
						break;
					case Schemas::Networking::MessageType_CONNECT_NAK:
						handle_connect_nak((Schemas::Networking::ConnectData*)(*msghdr).data());
						break;
					default:
						PRINT_ERR("net::client::thread: unknown message type " << (*msghdr).type());
						break;
					}
				} else {
					PRINT_ERR("net::client::thread: verify failed!");
				}
			}
		}
	});
	t.detach();

	attempt_connect();
}

void net::client::disconnect() {
	if (!m_connected)
		return;

	flatbuffers::FlatBufferBuilder fbb;

	Schemas::Networking::MessageHeaderBuilder mhb(fbb);
	mhb.add_type(Schemas::Networking::MessageType::MessageType_DISCONNECT);
	Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());

	send_to_server(fbb.GetBufferPointer(), fbb.GetSize());
#if defined(PLAT_WINDOWS)
	closesocket(m_socket);
#endif
}
