#pragma once

#include <math/vector.h>
#include <glm/fwd.hpp>

namespace gfx {
	class icamera {
	public:
		//  Move camera forward
		virtual void forward(float delta) = 0;
		//  Move camera backward
		virtual void backward(float delta) = 0;
		//  Move camera left and right
		virtual void strafe_left(float delta) = 0;
		virtual void strafe_right(float delta) = 0;
		// Turn camera left and right
		virtual void turn_left(float delta) = 0;
		virtual void turn_right(float delta) = 0;
		// Turn camera yaw and pitch
		virtual void turn(float x, float y, float delta) = 0;
		// Get camera position and rotation
		virtual math::vector3<float> get_pos() const = 0;
		virtual glm::mat4 get_rot() const = 0;
		// Called every frame
		virtual void update(float delta) = 0;
		virtual float yaw() const = 0;
		virtual float pitch() const = 0;
	};
}
