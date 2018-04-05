#pragma once

#include "networking.h"
#include <array>
#include <chrono>
#include <functional>
#include <string>

namespace net {
	struct client_desc {
		// Is this socket active
		bool slot_active = false;

		// Player's address and port
		sockaddr_in6 addr;
		// Player's name
		std::string username;
		// Time of last received packet from client
		std::chrono::time_point<std::chrono::high_resolution_clock> last_packet;
	};

	using server_handler = std::function<bool(const sockaddr_in6& client, const Schemas::Networking::MessageHeader& hdr, size_t siz)>;

	class server {
	public:
		server();
		~server();
		
		void set_max_players(size_t n) { if(n <= 256) m_max_players = n; }
		size_t get_max_players() const { return m_max_players; }

		void operator=(const server& other) = delete;
		server(const server& other) = delete;

		void on_connect();
		void update_state(const ent_id id, const edict_t& e);
		void broadcast_update();

		void add_handler(Schemas::Networking::MessageType t, server_handler& h) {
			m_handlers[t] = h;
		}

		void send_to_client(const sockaddr_in6& client, const void* pBuf, size_t nSiz);

		// MessageType::CONNECT
		bool handle_connection(const sockaddr_in6& client, const Schemas::Networking::MessageHeader& hdr, size_t siz);
		// MessageType::DISCOVERY_PROBE
		bool handle_discovery_probe(const sockaddr_in6& client, const Schemas::Networking::MessageHeader& hdr, size_t siz);

		net::socket_t get_socket() {
			return m_socket;
		}

		void add_handles();

		net::client_desc* get_client_desc(const sockaddr_in6& addr);
		net::client_desc* get_client_desc(const std::string_view& username);

	private:
		net::socket_t m_socket;
		bool m_listening = false;

		edict_t m_edicts[net::max_edicts];

		net::frame m_server_current_frame = 0;
		net::frame m_client_current_frame[net::max_players] = { 0 };

		net::world_update m_updates[net::stored_updates];

		std::array<net::client_desc, net::max_players> m_clients;

		size_t m_max_players = net::max_players;
		std::unordered_map<Schemas::Networking::MessageType, server_handler> m_handlers;
	};
}
