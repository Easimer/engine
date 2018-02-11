#include "stdafx.h"
#include "camera.h"

void camera::forward()
{
	RESTRICT_THREAD_LOGIC;
}

void camera::backward()
{
	RESTRICT_THREAD_LOGIC;
}

void camera::strafe_left()
{
	RESTRICT_THREAD_LOGIC;
}

void camera::strafe_right()
{
	RESTRICT_THREAD_LOGIC;
}

void camera::turn_left()
{
	RESTRICT_THREAD_LOGIC;
	m_vecRot[2] += m_flTurnSpeed * gpGlobals->flDeltaTime;
}

void camera::turn_right()
{
	RESTRICT_THREAD_LOGIC;
}

void camera::set_speed(float flSpeed)
{
	RESTRICT_THREAD_LOGIC;
	m_flSpeed = flSpeed;
}

void camera::set_turn_speed(float flTurnSpeed)
{
	RESTRICT_THREAD_LOGIC;
	m_flTurnSpeed = flTurnSpeed;
}

void camera::set_fov(float flFov)
{
	RESTRICT_THREAD_LOGIC;
	m_flFov = flFov;
}
