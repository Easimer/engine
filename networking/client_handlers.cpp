#include "stdafx.h"
#include <net/client.h>

void net::client::handle_connect_ack(const Schemas::Networking::ConnectData* pConnDat) {
	m_connected = true;
}

void net::client::handle_connect_nak(const Schemas::Networking::ConnectData* pConnDat) {
	if (pConnDat) {
		PRINT_ERR("net::client::thread: server refused connection: " << Schemas::Networking::EnumNameConnectionNakReason(pConnDat->nak_reason()));
	} else {
		PRINT_ERR("net::client::thread: server refused connection with no reason");
	}
	m_connected = false;
}
