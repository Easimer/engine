#include "stdafx.h"
#include "fps_player.h"
#include <glm/glm.hpp>

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

	glm::vec4 velocity(m_velocity[0], m_velocity[1], m_velocity[2], 0);
	velocity = (1.f / 33.f) * get_rotation_matrix() * velocity;
	vec3 tvel(velocity[0], velocity[1], velocity[2]);
	pos += tvel;

	set_abspos(pos);
}
