#pragma once

#include "vector.h"

class camera {
public:
	void forward();
	void backward();
	void strafe_left();
	void strafe_right();
	
	void turn_left();
	void turn_right();

	void set_speed(float flSpeed);
	void set_turn_speed(float flTurnSpeed);
	void set_fov(float flFov);

	vector& get_pos() { return m_vecPos; }
	vector& get_rot() { return m_vecRot; }

	void update();

private:
	vector m_vecPos;
	vector m_vecRot;
	float m_flSpeed, m_flTurnSpeed;
	float m_flFov;
};
