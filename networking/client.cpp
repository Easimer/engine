#include "stdafx.h"
#include <thread>
#include <net/client.h>

net::client::client(const std::string& addr, const std::string& username) : m_username(username) {
	socket_t s;
	
	int slen;

	slen = sizeof(m_server_addr);
#if defined(PLAT_WINDOWS)
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		PRINT_ERR("net::server::ctor: can't init WS: " << WSAGetLastError());
		return;
	}
#endif

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == invalid_socket) {
#if defined(PLAT_WINDOWS)
		PRINT_ERR("net::client::ctor: can't open socket: " << WSAGetLastError());
#elif defined(PLAT_LINUX)
		PRINT_ERR("net::client::ctor: can't open socket: " << strerror(errno));
#endif
		return;
	}

	m_socket = s;

	memset(&m_server_addr, 0, sizeof(m_server_addr));
	m_server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, addr.c_str(), &m_server_addr.sin_addr);
	m_server_addr.sin_port = htons(net::port);
	m_server_addr_siz = sizeof(m_server_addr);

	m_connected = false;
	timeout(1);
	connect();
}

net::client::~client() {
}

void net::client::send_update(const net::client_update & update) {
}

void net::client::push_client_updates() {
	while (!m_command_buf.empty()) {
		const std::string& cmd = m_command_buf.pop_back();

		flatbuffers::FlatBufferBuilder fbb;
		auto off_cmd = fbb.CreateString(cmd);
		Schemas::Networking::ClientInputBuilder cib(fbb);
		cib.add_command(off_cmd);
		cib.add_current_client_time(m_current_frame);
		auto off_payload = cib.Finish();
		Schemas::Networking::MessageHeaderBuilder mhb(fbb);
		mhb.add_type(Schemas::Networking::MessageType_CLIENT_UPDATE);
		mhb.add_data_type(Schemas::Networking::MessageData_ClientInput);
		mhb.add_data(off_payload.Union());
		Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());
		send_to_server(fbb.GetBufferPointer(), fbb.GetSize());
		{
			std::lock_guard<std::mutex> lg(m_current_stat_lock);
			m_current_stat.packet_siz[E_PKSTAT_CLIENT_UPDATE] += fbb.GetSize();
		}
	}
}

void net::client::send_to_server(const void * pBuf, size_t nSiz) {
	size_t sent = 0;
	if ((sent = sendto(m_socket, (char*)pBuf, nSiz, 0, (sockaddr*)&m_server_addr, m_server_addr_siz)) == net::socket_error) {
#if defined(PLAT_WINDOWS)
		auto iErr = WSAGetLastError();
		char buf[256];
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, iErr, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buf, 256, NULL);
		PRINT_ERR("net::client::sendtoclient: " << buf);
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
#if defined(PLAT_WINDOWS)
		PRINT_ERR("net::client::attempt_connect: sendto failed: " << WSAGetLastError());
#elif defined(PLAT_LINUX)
		PRINT_ERR("net::client::attempt_connect: sendto failed: " << strerror(errno));
#endif
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
		attempt_connect();
		std::chrono::time_point<std::chrono::steady_clock> pkstat_last_commit = std::chrono::steady_clock::now();
		while (true) {
			char buf[4096];
			net::socklen_t recv_len;
			sockaddr_in6 from;
			net::socklen_t slen = sizeof(from);
			if ((recv_len = recvfrom(socket, buf, 4096, 0, (sockaddr*)&from, &slen)) != net::socket_error) {
				//PRINT_DBG("net::client::thread: received " << recv_len << " bytes");
				auto verifier = flatbuffers::Verifier((const uint8_t*)buf, recv_len);
				if (Schemas::Networking::VerifyMessageHeaderBuffer(verifier)) {
					auto msghdr = Schemas::Networking::GetMessageHeader(buf);
					ASSERT(msghdr->Verify(verifier));
					m_current_frame = msghdr->tick();
					Schemas::Networking::MessageType msgtype = (*msghdr).type();
					switch (msgtype) {
					case Schemas::Networking::MessageType_ENTITY_UPDATE:
						handle_entity_update((Schemas::Networking::EntityUpdate*)(*msghdr).data());
						{
							std::lock_guard<std::mutex> lg(m_current_stat_lock);
							m_current_stat.packet_siz[E_PKSTAT_ENTITY_UPDATE] += recv_len;
						}
						break;
					case Schemas::Networking::MessageType_ENTITY_DELETE:
						handle_entity_deletion((Schemas::Networking::ULongIdentifier*)(*msghdr).data());
						{
							std::lock_guard<std::mutex> lg(m_current_stat_lock);
							m_current_stat.packet_siz[E_PKSTAT_OTHER] += recv_len;
						}
						break;
					case Schemas::Networking::MessageType_NONE:
						PRINT_ERR("net::client::thread: keepalive from server");
						{
							std::lock_guard<std::mutex> lg(m_current_stat_lock);
							m_current_stat.packet_siz[E_PKSTAT_OTHER] += recv_len;
						}
						break;
					case Schemas::Networking::MessageType_CONNECT_ACK:
						PRINT_ERR("net::client::thread: server acknowledges connection!");
						m_connected = true;
						break;
					case Schemas::Networking::MessageType_CONNECT_NAK:
						if(!m_connected) // check for duplicate conreq response
							handle_connect_nak((Schemas::Networking::ConnectData*)(*msghdr).data());
						break;
					case Schemas::Networking::MessageType_ECHO_REPLY:
						{
							std::lock_guard<std::mutex> lg(m_current_stat_lock);
							m_current_stat.packet_siz[E_PKSTAT_OTHER] += recv_len;
						}
						break;
					default:
						PRINT_ERR("net::client::thread: unknown message type " << Schemas::Networking::EnumNameMessageType(msgtype));
						break;
					}

					m_last_update = std::chrono::steady_clock::now();
				} else {
					PRINT_ERR("net::client::thread: verify failed!");
				}
			}

			if (std::chrono::duration<float>(std::chrono::steady_clock::now() - pkstat_last_commit).count() > 0.1) {
				std::lock_guard<std::mutex> lg(m_current_stat_lock);
				m_stats.push_front(m_current_stat);
				pkstat_last_commit = std::chrono::steady_clock::now();
				memset(&m_current_stat, 0, sizeof(m_current_stat));
			}
		}
	});
	t.detach();
}

void net::client::disconnect() {
	if (!m_connected)
		return;

	flatbuffers::FlatBufferBuilder fbb;

	Schemas::Networking::MessageHeaderBuilder mhb(fbb);
	mhb.add_type(Schemas::Networking::MessageType::MessageType_DISCONNECT);
	Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());

	send_to_server(fbb.GetBufferPointer(), fbb.GetSize());
	net::close_socket(m_socket);
}

void net::client::timeout(int sec, int usec) {
	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;

	if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		PRINT_ERR("net::client::timeout: failed to set recv timeout!");
	}
	if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		PRINT_ERR("net::client::timeout: failed to set send timeout!");
	}
}

bool net::client::timed_out() {
	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration<double>(now - m_last_update) >= timeout_threshold) {
		// Send ECHO_REQUEST
		flatbuffers::FlatBufferBuilder fbb;
		Schemas::Networking::MessageHeaderBuilder mhb(fbb);
		mhb.add_type(Schemas::Networking::MessageType::MessageType_ECHO_REQUEST);
		Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());
		send_to_server(fbb.GetBufferPointer(), fbb.GetSize());

		return true;
	}
	return false;
}

net::server_discovery::server_discovery() {
	char one = '1';
	constexpr size_t addr_len = sizeof(sockaddr_in6);
	size_t nSent;

#if defined(PLAT_WINDOWS)
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

	if ((m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == invalid_socket) {
#if defined(PLAT_WINDOWS)
		PRINT_ERR("net::client::discovery_probe: can't open socket: " << WSAGetLastError());
#elif defined(PLAT_LINUX)
		PRINT_ERR("net::client::discovery_probe: can't open socket: " << strerror(errno));
#endif
		return;
	}

	if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one)) < 0) {
		PRINT_ERR("net::client::discovery_probe: couldn't set broadcast option on socket!");
		net::close_socket(m_socket);
	}

	memset(&addr_rx, 0, addr_len);
	addr_rx.sin_family = AF_INET;
	addr_rx.sin_port = htons(net::port);
	//inet_pton(AF_INET6, "FF02::B1E5:5ED:BEEF", &addr_rx.sin6_addr.s6_addr);
	inet_pton(AF_INET, "255.255.255.255", &addr_rx.sin_addr);
}

net::server_discovery::~server_discovery() {
	net::close_socket(m_socket);
#if defined(PLAT_WINDOWS)
	WSACleanup();
#endif
}

void net::server_discovery::probe() {
	size_t nSent;
	constexpr int addr_len = sizeof(sockaddr_in6);
	flatbuffers::FlatBufferBuilder fbb;
	Schemas::Networking::MessageHeaderBuilder mhb(fbb);
	mhb.add_type(Schemas::Networking::MessageType::MessageType_DISCOVERY_PROBE);
	Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());

	for (int i = 0; i < 3; i++) {
		nSent = sendto(m_socket, (const char*)fbb.GetBufferPointer(), fbb.GetSize(), 0, (sockaddr*)&addr_rx, addr_len);
		ASSERT(nSent == fbb.GetSize());
		PRINT_DBG("net::server_discovery::probe: sent " << nSent << " bytes");
	}
}

void net::server_discovery::fetch() {
	sockaddr_in addr_tx;
	char buf[4096];
	int recv_len;
	net::socklen_t slen = sizeof(addr_tx);

	if ((recv_len = recvfrom(m_socket, buf, 4096, 0, (sockaddr*)&addr_tx, &slen)) != net::socket_error) {
		auto verifier = flatbuffers::Verifier((const uint8_t*)buf, recv_len);
		if (Schemas::Networking::VerifyMessageHeaderBuffer(verifier)) {
			auto msghdr = Schemas::Networking::GetMessageHeader(buf);
			ASSERT(msghdr->Verify(verifier));
			Schemas::Networking::MessageType msgtype = (*msghdr).type();
			if (msgtype == Schemas::Networking::MessageType_DISCOVERY_RESPONSE) {
				net::server_entry e;
				e.addr = addr_tx;
				if (msghdr->data_type() == Schemas::Networking::MessageData_ServerData && msghdr->data_as_ServerData()) {
					const Schemas::Networking::ServerData* srv_dat = msghdr->data_as_ServerData();
					e.name = srv_dat->name()->str();
					e.level = srv_dat->level()->str();
					e.players = srv_dat->players();
					e.max_players = srv_dat->max_players();
				}
				m_discovered_servers.push_back(e);
			}
		}
	}
}

void net::server_discovery::timeout(int sec, int usec) {
	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;

	if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		PRINT_ERR("net::server_discovery::timeout: failed to set recv timeout!");
	}
	if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		PRINT_ERR("net::server_discovery::timeout: failed to set send timeout!");
	}
}
