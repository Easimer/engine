#include "stdafx.h"
#include "camera.h"
#include "renderer.h"
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

void camera::forward()
{
	RESTRICT_THREAD_LOGIC;

	auto matRot = glm::eulerAngleXYZ(m_vecRot[0], m_vecRot[1], m_vecRot[2]);
	auto vecForward = glm::vec4(0, 0, 1, 0);
	vecForward = matRot * vecForward;
	m_vecPos = m_vecPos + vec3(vecForward) * gpGlobals->flDeltaTime * m_flSpeed;
	gpGlobals->pStatistics->get_stat_fl(ESTAT_C_GAME, "player speed") = gpGlobals->flDeltaTime * m_flSpeed;
}

void camera::backward()
{
	RESTRICT_THREAD_LOGIC;

	auto matRot = glm::eulerAngleXYZ(m_vecRot[0], m_vecRot[1], m_vecRot[2]);
	auto vecBackward = glm::vec4(0, 0, 1, 0);
	vecBackward = matRot * vecBackward;
	m_vecPos = m_vecPos - vec3(vecBackward) * gpGlobals->flDeltaTime * m_flSpeed;
	gpGlobals->pStatistics->get_stat_fl(ESTAT_C_GAME, "player speed") = gpGlobals->flDeltaTime * m_flSpeed;
}

void camera::strafe_left()
{
	RESTRICT_THREAD_LOGIC;

	auto matRot = glm::eulerAngleXYZ(m_vecRot[0], m_vecRot[1], m_vecRot[2]);
	auto vecLeft = glm::vec4(1, 0, 0, 0);
	vecLeft = matRot * vecLeft;
	m_vecPos = m_vecPos + vec3(vecLeft) * gpGlobals->flDeltaTime * m_flSpeed;
	gpGlobals->pStatistics->get_stat_fl(ESTAT_C_GAME, "player speed") = gpGlobals->flDeltaTime * m_flSpeed;
}

void camera::strafe_right()
{
	RESTRICT_THREAD_LOGIC;

	auto matRot = glm::eulerAngleXYZ(m_vecRot[0], m_vecRot[1], m_vecRot[2]);
	auto vecRight = glm::vec4(-1, 0, 0, 0);
	vecRight = matRot * vecRight;
	m_vecPos = m_vecPos + vec3(vecRight) * gpGlobals->flDeltaTime * m_flSpeed;
	gpGlobals->pStatistics->get_stat_fl(ESTAT_C_GAME, "player speed") = gpGlobals->flDeltaTime * m_flSpeed;
}

void camera::turn_left()
{
	RESTRICT_THREAD_LOGIC;
	m_vecRot[1] += m_flTurnSpeed * gpGlobals->flDeltaTime;
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
