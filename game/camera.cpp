#include "stdafx.h"
#include "renderer.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

void camera::forward()
{
	RESTRICT_THREAD_LOGIC;

	glm::vec3 vecFwd3 = glm::vec3(m_matRot[0][2], m_matRot[1][2], m_matRot[2][2]);
	m_vecPos = m_vecPos + vecFwd3 * (float)(m_flSpeed * gpGlobals->flTickTime);
}

void camera::backward()
{
	RESTRICT_THREAD_LOGIC;

	glm::vec3 vecFwd3 = glm::vec3(m_matRot[0][2], m_matRot[1][2], m_matRot[2][2]);
	m_vecPos = m_vecPos - vecFwd3 * (float)(m_flSpeed * gpGlobals->flTickTime);
}

void camera::strafe_left()
{
	RESTRICT_THREAD_LOGIC;

	glm::vec3 vecRight = glm::vec3(m_matRot[0][0], m_matRot[1][0], m_matRot[2][0]);
	m_vecPos = m_vecPos + vecRight * (float)(m_flSpeed * gpGlobals->flTickTime);
}

void camera::strafe_right()
{
	RESTRICT_THREAD_LOGIC;

	glm::vec3 vecRight = glm::vec3(m_matRot[0][0], m_matRot[1][0], m_matRot[2][0]);
	m_vecPos = m_vecPos - vecRight * (float)(m_flSpeed * gpGlobals->flTickTime);
}

void camera::turn_left()
{
	RESTRICT_THREAD_LOGIC;

	m_matRot = glm::rotate(m_matRot, m_flTurnSpeed * (float)gpGlobals->flTickTime, glm::vec3(0, 1, 0));
}

void camera::turn_right()
{
	RESTRICT_THREAD_LOGIC;

	m_matRot = glm::rotate(m_matRot, -m_flTurnSpeed * (float)gpGlobals->flTickTime, glm::vec3(0, 1, 0));
}

void camera::turn(float x, float y)
{
	// X is yaw
	// Y is pitch

	float dx = x * m_flTurnSpeed * gpGlobals->flTickTime;
	float dy = y * m_flTurnSpeed * gpGlobals->flTickTime;

	m_flYaw += dx;
	m_flPitch += dy;

	m_flPitch = glm::clamp(m_flPitch, glm::radians(-90.0f), glm::radians(90.0f));

	m_matRot = glm::rotate(glm::mat4(1.0), m_flYaw, glm::vec3(0, 1, 0));
	m_matRot = glm::rotate(m_matRot, m_flPitch, glm::vec3(1, 0, 0));
	
}

vector camera::get_rot() const
{
	//return vector(glm::eulerAngles(m_matRot));
	return vector(m_flPitch, m_flYaw, 0);
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
	gpGlobals->pRenderer->update_camera(m_vecPos, m_matRot);
}
