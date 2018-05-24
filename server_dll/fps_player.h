#pragma once

#include "base_player.h"

class fps_player : public base_player {
public:
	DEC_CLASS(fps_player, base_player);
	virtual ~fps_player() {};

	virtual void spawn();

	vec3& velocity() { return m_velocity; }
	const vec3& velocity() const { return m_velocity; }

	void think();

	BEGIN_KEYVALUES(fps_player)
	END_KEYVALUES()
private:
	vec3 m_velocity;
};
