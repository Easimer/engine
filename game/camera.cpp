#include "stdafx.h"
#include "camera.h"
#include "renderer.h"

void camera::forward()
{
	RESTRICT_THREAD_LOGIC;
	float flRot = m_vecRot[2];
	m_vecPos[0] = m_flSpeed * cos(flRot) * gpGlobals->flDeltaTime;
	m_vecPos[1] = m_flSpeed * sin(flRot) * gpGlobals->flDeltaTime;

	PRINT_DBG("POS: " << m_vecPos[0] << ' ' << m_vecPos[1]);
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

void camera::update()
{
	gpGlobals->pRenderer->update_camera(m_vecPos, m_vecRot);
}
