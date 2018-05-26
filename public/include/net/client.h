#pragma once

#include "networking.h"
#include <chrono>
#include <ring_buffer.h>
#include <mutex>

namespace net {

	using client_handler = std::function<bool(const Schemas::Networking::MessageHeader& hdr, size_t siz)>;

	class client {
	public:
		client(const std::string& addr, const std::string& username);
		~client();

		enum connection_status {
			E_CLIENT_CONNSTATUS_OK = 0,
			E_CLIENT_CONNSTATUS_FAIL = 1,
			E_CLIENT_CONNSTATUS_ALRDCONN = 2,
			E_CLIENT_CONNSTATUS_NAME_UNAVAIL = 3,
			E_CLIENT_CONNSTATUS_FULL = 4,
		};

		void operator=(const client& other) = delete;
		client(const client& other) = delete;

		void send_update(const net::client_update& update);
		void push_client_updates();

		void send_to_server(const void* pBuf, size_t nSiz);

		net::socket_t get_socket() {
			return m_socket;
		}

		void add_handlers();

		void attempt_connect();
		void connect();

		bool connected() const { return m_connected; }

		void handle_connect_ack(const Schemas::Networking::ConnectData* pConnDat);
		void handle_connect_nak(const Schemas::Networking::ConnectData* pConnDat);
		void handle_entity_update(const Schemas::Networking::EntityUpdate* pEntUpd);
		void handle_entity_deletion(const Schemas::Networking::ULongIdentifier* pEntUpd);

		void disconnect();

		const edict_t* const get_edicts() const { return m_edicts; }
		edict_t* const get_edicts() { return m_edicts; }

		const std::chrono::duration<double> timeout_threshold = std::chrono::duration<double>(3);

		void timeout(int secs = 0, int usecs = 0);

		// Returns whether the server has timed out and
		// sends an ECHO_REQUEST to it
		bool timed_out();

		ring_buffer<std::string, 128>& get_command_buf() { return m_command_buf; }
		const ring_buffer<net::packet_stat, 128>& get_packet_stats() const { return m_stats; }
		ring_buffer<net::packet_stat, 128>& get_packet_stats() { return m_stats; }

	private:
		net::socket_t m_socket;
		bool m_connected = false;

		edict_t m_edicts[net::max_edicts];
		net::frame m_current_frame = 0;

		sockaddr_in m_server_addr;
		int m_server_addr_siz;

		std::string m_username;
		std::unordered_map<Schemas::Networking::MessageType, client_handler> m_handlers;

		std::chrono::time_point<std::chrono::steady_clock> m_last_update;

		ring_buffer<std::string, 128> m_command_buf;

		net::packet_stat m_current_stat;
		std::mutex m_current_stat_lock;
		ring_buffer<net::packet_stat, 128> m_stats;
	};

	struct server_entry {
		sockaddr_in addr;
		std::string name;
		std::string level;
		uint64_t players;
		uint64_t max_players;
	};

	class server_discovery {
	public:
		server_discovery();
		~server_discovery();
		void probe();
		void fetch();
		const std::vector<server_entry> get() const { return m_discovered_servers; }
		void timeout(int sec = 0, int usec = 0);
	private:
		socket_t m_socket;
		sockaddr_in addr_rx;
		std::vector<server_entry> m_discovered_servers;
	};
}

inline bool operator==(const net::server_entry& lhs, const net::server_entry& rhs) {
	return lhs.addr == rhs.addr;
}
