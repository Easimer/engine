#include "stdafx.h"
#include <phys/phys.h>
#include <phys/object.h>
#include <print.h>

void phys::object::integrate(float delta)
{
	vector3<float> diff = delta * m_velocity;
	m_position += diff;
	m_collider.transform(diff);
}
