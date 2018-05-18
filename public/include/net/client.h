#pragma once

#include "networking.h"

namespace net {

	using client_handler = std::function<bool(const Schemas::Networking::MessageHeader& hdr, size_t siz)>;

	class client {
	public:
		client(const std::string& addr, const std::string& username);
		~client();

		void operator=(const client& other) = delete;
		client(const client& other) = delete;

		void send_update(const net::client_update& update);

		void send_to_server(const void* pBuf, size_t nSiz);

		net::socket_t get_socket() {
			return m_socket;
		}

		void add_handlers();

		void attempt_connect();
		void connect();

		bool connected() const { return m_connected; }

		void discovery_probe();
		const std::vector<sockaddr_in6>& get_discovered_servers() const { return m_discovered_servers; }

		void handle_connect_ack(const Schemas::Networking::ConnectData* pConnDat);
		void handle_connect_nak(const Schemas::Networking::ConnectData* pConnDat);
		void handle_entity_update(const Schemas::Networking::EntityUpdate* pEntUpd);
		void handle_discovery_response(const sockaddr_in6 saddr, const unsigned short port);

		void disconnect();

	private:
		net::socket_t m_socket;
		bool m_connected = false;

		edict_t m_edicts[net::max_edicts];
		net::frame m_current_frame = 0;

		sockaddr_in6 m_server_addr;
		int m_server_addr_siz;

		std::string m_username;
		std::unordered_map<Schemas::Networking::MessageType, client_handler> m_handlers;

		std::vector<sockaddr_in6> m_discovered_servers;
	};
}
