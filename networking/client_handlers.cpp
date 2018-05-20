﻿#include "stdafx.h"
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

	float dts = abs(pEntUpd->last_update() - e.last_update); // Δt_s [Δs]
	PRINT_DBG(dts);
	e.last_update = pEntUpd->last_update();
	auto pos = pEntUpd->pos();
	if (pos) {
		float x = pos->x();
		float y = pos->y();
		float z = pos->z();

		// Calculate error
		float ex = (x - e.iposition[0]);
		float ey = (y - e.iposition[1]);
		float ez = (z - e.iposition[2]);
		float err = vec3(ex, ey, ez).length();
		PRINT_DBG("Error: " << err);

		if (dts != 0) {
			float flDeltaX = (x - e.position[0]);
			float flDeltaY = (y - e.position[1]);
			float flDeltaZ = (z - e.position[2]);
			vec3 vel(flDeltaX / dts, flDeltaY / dts, flDeltaZ / dts);
			e.iacceleration = (vel - e.velocity) / dts;
			e.velocity = vel;
			
			//if (err < 0.2)
			//	e.ivelocity = e.velocity + vec3(ex / dts, ey / dts, ez / dts); // reset interpolated velocity
			//else
				e.ivelocity = e.velocity;
		} else {
			e.velocity[0] = 0;
			e.velocity[1] = 0;
			e.velocity[2] = 0;
		}
		e.position[0] = x;
		e.position[1] = y;
		e.position[2] = z;
		e.iposition[0] = x;
		e.iposition[1] = y;
		e.iposition[2] = z;
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
