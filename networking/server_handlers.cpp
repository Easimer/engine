#include "stdafx.h"
#include <net/server.h>

void net::server::add_handles() {
	// NONE
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_NONE,
		[&](const sockaddr_in6& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		send_to_client(client, &hdr, siz);
		return true;
	});

	// CONNECT
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_CONNECT,
		[&](const sockaddr_in6& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
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
		[&](const sockaddr_in6& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
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
		[&](const sockaddr_in6& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		return false;
	});

	// DISCOVERY_PROBE
	m_handlers.emplace(Schemas::Networking::MessageType::MessageType_DISCOVERY_PROBE,
		[&](const sockaddr_in6& client, const Schemas::Networking::MessageHeader& hdr, size_t siz) {
		flatbuffers::FlatBufferBuilder fbb;
		Schemas::Networking::MessageHeaderBuilder mhb(fbb);
		mhb.add_type(Schemas::Networking::MessageType_DISCOVERY_RESPONSE);
		Schemas::Networking::FinishMessageHeaderBuffer(fbb, mhb.Finish());
		for(int i = 0; i < 3; i++)
			send_to_client(client, fbb.GetBufferPointer(), fbb.GetSize());
		return true;
	});
}
