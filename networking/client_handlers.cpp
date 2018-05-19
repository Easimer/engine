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

void net::client::handle_entity_update(const Schemas::Networking::EntityUpdate* pEntUpd) {
	//PRINT_DBG("net::client::handle_entity_update: edict=" << pEntUpd->edict_id() << " pos: (" << pEntUpd->pos()->x() << ',' << pEntUpd->pos()->y() << ',' << pEntUpd->pos()->z() << ')');
	auto& e = m_edicts[pEntUpd->edict_id()];
	e.active = true;
	e.updated = false;
	auto pos = pEntUpd->pos();
	if (pos) {
		e.position[0] = pos->x();
		e.position[1] = pos->y();
		e.position[2] = pos->z();
	}
	auto rot = pEntUpd->rot();
	if (rot) {
		e.rotation2[0] = rot->x();
		e.rotation2[1] = rot->y();
		e.rotation2[2] = rot->z();
	}
	auto model = pEntUpd->model();
	if (model) {
		strncpy(e.modelname, model->c_str(), 128);
	}
}
