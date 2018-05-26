#pragma once

#include "networking.h"
#include <array>
#include <chrono>
#include <functional>
#include <string>
#include <mutex>
#include <atomic>

#define NETWORKING_SERVER_PERIODIC_WORLD_UPDATES 1

namespace net {
	struct client_desc {
		// Is this socket active
		bool slot_active = false;

		// Player's address and port
		sockaddr_in addr;
		// Player's name
		std::string username;
		// Time of last received packet from client
		std::chrono::time_point<std::chrono::high_resolution_clock> last_packet;
	};

	struct entity_update {
		entity_update() : model(nullptr) {}
		size_t edict;
		float pos[3];
		float rot[16];
		const char* model;
		float time;
		bool deleted = false;
	};

	using server_handler = std::function<bool(const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz)>;
	using client_input_handler_t = std::function<void(const net::client_desc& client, size_t edict, uint64_t cli_tick, const char* pszCommand)>;

	class server {
	public:
		server();
		~server();
		
		// Set max player count
		// Shall not be set to a value greater
		// than the hardcoded player limit
		// (net::max_players)
		void set_max_players(size_t n) {  m_max_players = (n <= net::max_players) ? n : net::max_players; }
		// Get max player count
		size_t get_max_players() const { return m_max_players; }

		void operator=(const server& other) = delete;
		server(const server& other) = delete;

		void on_connect();
		void update_state(const ent_id id, const edict_t& e);

		void unicast_update(const entity_update& upd, const net::client_desc& cd);
		void broadcast_update(const entity_update& upd);

		// Add packet handler for MessageType `t'
		// See net::server_handler for function signature
		void add_handler(Schemas::Networking::MessageType t, server_handler& h) {
			m_handlers[t] = h;
		}

		// Sends `nSiz' bytes of data from `pBuf' to client with address `client'
		void send_to_client(const sockaddr_in& client, const void* pBuf, size_t nSiz);

		// Get the socket handle
		net::socket_t get_socket() {
			return m_socket;
		}

		void add_handles();

		// Get client descriptor by address
		net::client_desc* get_client_desc(const sockaddr_in& addr);
		// Get client descriptor by username
		net::client_desc* get_client_desc(const std::string_view& username);
		// Get client descriptor by index
		net::client_desc* get_client_desc(size_t i) { if (i >= net::max_players) return nullptr; return &m_clients[i]; }

		// Get ref to edict on index `id'
		inline net::edict_t& edict(const ent_id id) {
			return m_edicts[id];
		}

		// Set value of edict on index `id'
		inline void edict(const ent_id id, const edict_t& e) {
			m_edicts[id] = e;
		}

		// Push pending updates to all clients
		void push_updates();
		// Push full world update to a client
		void push_full_update(const net::client_desc&);

		// Set the pointer from where the server receiver
		// thread gets the current engine time
		void set_time_ptr(const float* pCurTime) { m_pCurTime = pCurTime; }

		// Get player count
		size_t player_count() const;

		// Set server display name
		void set_name(const std::string& str) { m_server_name = str; }
		// Set display level name
		void set_level_name(const std::string& str) { m_level_name = str; }

		// Get player edict by their index in the clients
		// array.
		net::edict_t& get_player(const size_t i) {
			// The first net::max_players valid edicts are
			// the ones reserved for players, but 
			// edict 0 is defined as invalid.
			return m_edicts[i + 1];
		}

		void inc_frame() { m_server_current_frame++; }

		void set_client_input_handler(const client_input_handler_t& f) {
			m_client_input_handler = f;
		}

		size_t packets_per_sec() const { return m_nStatPacketsPerSec; }

	private:
		net::socket_t m_socket;
		bool m_listening = false;

		edict_t m_edicts[net::max_edicts];

		net::frame m_server_current_frame = 0;

		net::world_update m_updates[net::stored_updates];

		std::array<net::client_desc, net::max_players> m_clients;

		size_t m_max_players = net::max_players;
		std::unordered_map<Schemas::Networking::MessageType, server_handler> m_handlers;

		const float* m_pCurTime;

		std::string m_server_name;
		std::string m_level_name;

		client_input_handler_t  m_client_input_handler;

		// statistics
		// Packets sent and received since m_nStatPacketsLastCheck
		std::atomic<size_t> m_nStatPackets = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_nStatPacketsLastCheck = std::chrono::high_resolution_clock::now();
		// Packets sent and received in the last second
		size_t m_nStatPacketsPerSec = 0;
	};
}
