#include "stdafx.h"
#include <gfx/camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void gfx::camera::forward(float delta)
{
	glm::vec3 vecFwd3 = glm::vec3(m_matRot[0][2], m_matRot[1][2], m_matRot[2][2]);
	m_vecPos = m_vecPos + vecFwd3 * (float)(m_flSpeed * delta);
}

void gfx::camera::backward(float delta)
{
	glm::vec3 vecFwd3 = glm::vec3(m_matRot[0][2], m_matRot[1][2], m_matRot[2][2]);
	m_vecPos = m_vecPos - vecFwd3 * (float)(m_flSpeed * delta);
}

void gfx::camera::strafe_left(float delta)
{
	glm::vec3 vecRight = glm::vec3(m_matRot[0][0], m_matRot[1][0], m_matRot[2][0]);
	m_vecPos = m_vecPos + vecRight * (float)(m_flSpeed * delta);
}

void gfx::camera::strafe_right(float delta)
{
	glm::vec3 vecRight = glm::vec3(m_matRot[0][0], m_matRot[1][0], m_matRot[2][0]);
	m_vecPos = m_vecPos - vecRight * (float)(m_flSpeed * delta);
}

void gfx::camera::turn_left(float delta)
{
}

void gfx::camera::turn_right(float delta)
{
}

void gfx::camera::turn(float x, float y, float delta)
{
	glm::vec3 vecFwd = glm::vec3(m_matRot[0][2], m_matRot[1][2], m_matRot[2][2]);
	glm::vec3 vecUp = glm::vec3(m_matRot[0][0], m_matRot[1][0], m_matRot[2][0]);

	float dx = x * m_flTurnSpeed * 0.2 * delta;
	float dy = y * m_flTurnSpeed * 0.2 * delta;

	m_flPitch += dy;

	m_flPitch = glm::clamp(m_flPitch, glm::radians(-90.0f), glm::radians(90.0f));

	if ((m_flPitch > glm::radians(90.0f) && m_flPitch < glm::radians(270.0f)) || (m_flPitch < glm::radians(-90.0f) && m_flPitch > glm::radians(-270.0f)))
	{
		m_flYaw -= dx;
	}
	else
	{
		m_flYaw += dx;
	}

	m_matRot = glm::rotate(glm::mat4(1.0), m_flPitch, glm::vec3(1, 0, 0));
	m_matRot = glm::rotate(m_matRot, m_flYaw, glm::vec3(0, 1, 0));
}

math::vector3<float> gfx::camera::get_pos() const
{
	return math::vector3<float>(m_vecPos);
}

glm::mat4 gfx::camera::get_rot() const
{
	return m_matRot;
}

void gfx::camera::update(float delta)
{
	
}

float gfx::camera::yaw() const
{
	return m_flYaw;
}

float gfx::camera::pitch() const
{
	return m_flPitch;
}
