#include "stdafx.h"
#include <net/server.h>

void net::server::add_handles() {
	// NONE
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_NONE,
		[&](const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		send_to_client(client, &hdr, siz);
		return true;
	});

	// CONNECT
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_CONNECT,
		[&](const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		// Check if user is already connected
		bool ack = true;
		int first_free_slot = -1;
		size_t max_players = get_max_players();
		Schemas::Networking::ConnectionNakReason nak_reason = Schemas::Networking::ConnectionNakReason_RESERVED;

		auto pConnData = (Schemas::Networking::ConnectData*)hdr.data();

		PRINT_DBG("net::server::handle_connection: client attemps to connect with name " << pConnData->username()->str());

		for (size_t i = 0; i < max_players; i++) {
			const net::client_desc& c = m_clients[i];

			// Skip free slots
			if (!c.slot_active) {
				if (first_free_slot == -1)
					first_free_slot = i;
				continue;
			}

			// Already connected
			if (c.addr == client) {
				ack = false;
				nak_reason = Schemas::Networking::ConnectionNakReason_ALREADY_CONNECTED;
				break;
			}
			// Name is unavailable
			if (c.username == pConnData->username()->str()) {
				ack = false;
				nak_reason = Schemas::Networking::ConnectionNakReason_NAME_UNAVAILABLE;
				break;
			}
		}
		// Server is full
		if (ack && first_free_slot == -1) {
			ack = false;
			nak_reason = Schemas::Networking::ConnectionNakReason_FULL;
		}


		// Build packet
		flatbuffers::FlatBufferBuilder fbb;
		Schemas::Networking::ConnectDataBuilder cdb(fbb);
		cdb.add_nak_reason(nak_reason); // NAK reason (if any)

		const net::client_desc* cd = nullptr;

		if (first_free_slot != -1) {
			net::client_desc& c = m_clients[first_free_slot];
			cdb.add_conn_id(first_free_slot);

			c.username = pConnData->username()->str();
			c.last_packet = std::chrono::high_resolution_clock::now();
			c.slot_active = true;
			memcpy(&c.addr, &client, sizeof(client));
			PRINT_DBG("Inserted PLAYER INTO DESCS");
			cd = &c;
		}

		auto cdb_off = cdb.Finish();

		Schemas::Networking::MessageHeaderBuilder mhb(fbb);
		mhb.add_data(cdb_off.Union()); // Add ConnectData
		mhb.add_data_type(Schemas::Networking::MessageData::MessageData_ConnectData);
		if (ack) {
			mhb.add_type(Schemas::Networking::MessageType::MessageType_CONNECT_ACK);
			mhb.add_tick(m_server_current_frame);
		} else {
			mhb.add_type(Schemas::Networking::MessageType::MessageType_CONNECT_NAK);
		}
		Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());

		for(int i = 0; i < 3; i++)
			send_to_client(client, fbb.GetBufferPointer(), fbb.GetSize());

		if(cd)
			push_full_update(*cd);

		return true;
	});
	
	// DISCONNECT
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_DISCONNECT,
		[&](const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		auto pClientDesc = get_client_desc(client);
		if (pClientDesc) {
			pClientDesc->slot_active = false;
			PRINT_DBG("net::server::thread: player " << pClientDesc->username << " has disconnected");
			return true;
		} else {
			return false;
		}
	});

	// CLIENT_UPDATE
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_CLIENT_UPDATE,
		[&](const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		// Get client descriptor
		net::client_desc* cd = get_client_desc(client);
		if (!cd)
			return false;
		if (!cd->slot_active)
			return false;
		// Calculate descriptor index
		size_t i = ((size_t)cd - (size_t)m_clients.data()) / sizeof(net::client_desc);
		//net::edict_t& e = get_player(i);
		// TODO: we should send this up to the control plane and handle it there for e.g.
		// checking gamerules and anti-cheat
		auto client_input = hdr.data_as_ClientInput();
		if (!client_input)
			return false;
		if (!client_input->command())
			return false;
		if (m_client_input_handler) {
			m_client_input_handler(*cd, i + 1, hdr.tick(), client_input->command()->c_str());
			return true;
		}
		return false;
	});

	// DISCOVERY_PROBE
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_DISCOVERY_PROBE,
		[&](const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		flatbuffers::FlatBufferBuilder fbb;
		auto srv_name = fbb.CreateString(m_server_name);
		auto srv_level = fbb.CreateString(m_level_name);
		Schemas::Networking::ServerDataBuilder sdb(fbb);
		sdb.add_name(srv_name);
		sdb.add_level(srv_level);
		sdb.add_players(player_count());
		sdb.add_max_players(max_players);
		auto srv_dat = sdb.Finish();
		Schemas::Networking::MessageHeaderBuilder mhb(fbb);
		mhb.add_type(Schemas::Networking::MessageType_DISCOVERY_RESPONSE);
		mhb.add_data_type(Schemas::Networking::MessageData_ServerData);
		mhb.add_data(srv_dat.Union());
		Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());
		for (int i = 0; i < 3; i++)
			send_to_client(client, fbb.GetBufferPointer(), fbb.GetSize());
		return true;
	});

	// ECHO_REQUEST
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_ECHO_REQUEST,
		[&](const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		flatbuffers::FlatBufferBuilder fbb;
		Schemas::Networking::MessageHeaderBuilder mhb(fbb);
		mhb.add_type(Schemas::Networking::MessageType_ECHO_REPLY);
		Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());
		for (int i = 0; i < 3; i++)
			send_to_client(client, fbb.GetBufferPointer(), fbb.GetSize());
		return true;
	});

	// QUERY_REQUEST
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_QUERY_REQUEST,
		[&](const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		flatbuffers::FlatBufferBuilder fbb;
		auto srv_name = fbb.CreateString(m_server_name);
		auto srv_level = fbb.CreateString(m_level_name);
		Schemas::Networking::ServerDataBuilder sdb(fbb);
		sdb.add_name(srv_name);
		sdb.add_level(srv_level);
		sdb.add_players(player_count());
		sdb.add_max_players(max_players);
		auto srv_dat = sdb.Finish();
		Schemas::Networking::MessageHeaderBuilder mhb(fbb);
		mhb.add_type(Schemas::Networking::MessageType_QUERY_REPLY);
		mhb.add_data_type(Schemas::Networking::MessageData_ServerData);
		mhb.add_data(srv_dat.Union());
		Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());
		for (int i = 0; i < 3; i++)
			send_to_client(client, fbb.GetBufferPointer(), fbb.GetSize());
		return true;
	});

	// SYNC_CLOCK
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_SYNC_CLOCK,
		[&](const sockaddr_in& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		flatbuffers::FlatBufferBuilder fbb;
		return false;
	});
}
