#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "icamera.h"

class camera : public icamera {
public:
	void forward() override;
	void backward() override;
	void strafe_left() override;
	void strafe_right() override;
	
	void turn_left() override;
	void turn_right() override;

	void turn(float x, float y) override;

	vector get_pos() const override { return vector(m_vecPos); }
	vector get_rot() const override;

	void set_speed(float flSpeed);
	void set_turn_speed(float flTurnSpeed);
	void set_fov(float flFov);

	void update() override;

private:
	glm::vec3 m_vecPos;
	glm::mat4 m_matRot = glm::mat4(1.0);
	float m_flSpeed = 0.8, m_flTurnSpeed = 3.3;
	float m_flFov;
	
	float m_flYaw = 0.0;
	float m_flPitch = 0.0;

	bool m_bSprinting = false;
};
