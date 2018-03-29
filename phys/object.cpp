#include "stdafx.h"
#include <phys/phys.h>
#include <phys/object.h>
#include <print.h>

void phys::object::integrate(float delta)
{
	math::vector3<float> diff = delta * m_velocity;
	m_position += diff;
}
