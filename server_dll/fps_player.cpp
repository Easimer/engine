#include "stdafx.h"
#include "fps_player.h"

REGISTER_ENTITY(fps_player, player);

void fps_player::spawn() {
	BaseClass::precache();
	BaseClass::spawn();

	SetThink(&fps_player::think);
	SetNextThink(0); // Think every tick
}

void fps_player::think() {
	// apply velocity
	vec3 pos = get_abspos();

	pos += (1.f / 33.f) * m_velocity;

	set_abspos(pos);
}
