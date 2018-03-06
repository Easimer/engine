#pragma once

#include "vector.h"

// camera interface so we won't have to include GLM headers in every
// other headers using the camera

class icamera {
public:
	// Movement
	virtual void forward() = 0;
	virtual void backward() = 0;

	// Strafing
	virtual void strafe_left() = 0;
	virtual void strafe_right() = 0;

	// Keyboard look
	virtual void turn_left() = 0;
	virtual void turn_right() = 0;
	// Mouse look
	virtual void turn(float x, float y) = 0;

	virtual vector get_pos() const = 0;
	// Returns Euler angles
	virtual vector get_rot() const = 0;

	virtual void update() = 0;

	virtual void set_speed(float flNewSpeed) = 0;
};
