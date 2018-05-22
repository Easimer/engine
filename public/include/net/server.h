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
	};

	using server_handler = std::function<bool(const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz)>;

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

		void unicast_update(const entity_update& upd, const net::client_desc& cd);
		void broadcast_update(const entity_update& upd);

		void add_handler(Schemas::Networking::MessageType t, server_handler& h) {
			m_handlers[t] = h;
		}

		void send_to_client(const sockaddr_in& client, const void* pBuf, size_t nSiz);

		net::socket_t get_socket() {
			return m_socket;
		}

		void add_handles();

		net::client_desc* get_client_desc(const sockaddr_in& addr);
		net::client_desc* get_client_desc(const std::string_view& username);

		inline net::edict_t& edict(const ent_id id) {
			return m_edicts[id];
		}

		inline void edict(const ent_id id, const edict_t& e) {
			m_edicts[id] = e;
		}

		void push_updates();
		void push_full_update(const net::client_desc&);

		void set_time_ptr(const float* pCurTime) { m_pCurTime = pCurTime; }

		size_t player_count() const;

		void set_name(const std::string& str) { m_server_name = str; }
		void set_level_name(const std::string& str) { m_level_name = str; }

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

		const float* m_pCurTime;

		std::string m_server_name;
		std::string m_level_name;
	};
}
