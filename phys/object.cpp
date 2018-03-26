#include "stdafx.h"
#include <phys/phys.h>
#include <phys/object.h>

void phys::object::integrate(float delta)
{
	m_position += delta * m_velocity;
}
