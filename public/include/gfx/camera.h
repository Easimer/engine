#pragma once
#include <gfx/icamera.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace gfx {
	class camera : public icamera {
	public:
		camera() :
			m_matRot(glm::mat4(1.0)),
			m_flSpeed(0.8),
			m_flTurnSpeed(3.3),
			m_flYaw(0.0),
			m_flPitch(0.0),
			m_bSprinting(false) {}

		virtual void forward(float delta) override;
		virtual void backward(float delta) override;
		virtual void strafe_left(float delta) override;
		virtual void strafe_right(float delta) override;
		virtual void turn_left(float delta) override;
		virtual void turn_right(float delta) override;
		virtual void turn(float x, float y, float delta) override;
		virtual math::vector3<float> get_pos() const override;
		virtual glm::mat4 get_rot() const override;
		virtual void update(float delta) override;
		virtual float yaw() const override;
		virtual float pitch() const override;

		virtual void set_speed(float speed) {
			m_flSpeed = speed;
		}

	protected:
		glm::vec3 m_vecPos;
		glm::mat4 m_matRot = glm::mat4(1.0);
		float m_flSpeed = 0.8, m_flTurnSpeed = 3.3;
		float m_flFov;

		float m_flYaw = 0.0;
		float m_flPitch = 0.0;

		bool m_bSprinting = false;
	};
}
