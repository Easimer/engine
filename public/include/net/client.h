#pragma once

#include "networking.h"

namespace net {
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

		void attempt_connect();
		void connect();

		bool connected() const { return m_connected; }

		void handle_connect_ack(const Schemas::Networking::ConnectData* pConnDat);
		void handle_connect_nak(const Schemas::Networking::ConnectData* pConnDat);

		void disconnect();

	private:
		net::socket_t m_socket;
		bool m_connected = false;

		edict_t m_world_state[net::max_edicts];

		sockaddr_in6 m_server_addr;
		int m_server_addr_siz;

		std::string m_username;
	};
}
