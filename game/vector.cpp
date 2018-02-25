#include "stdafx.h"
#include <iostream>
#include "vector.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

vector::vector(const glm::vec3& v)
{
	m_aflValues[0] = v[0];
	m_aflValues[1] = v[1];
	m_aflValues[2] = v[2];
}

vector::vector(const glm::vec4& v)
{
	m_aflValues[0] = v[0];
	m_aflValues[1] = v[1];
	m_aflValues[2] = v[2];
}

void vector::operator=(const glm::vec3& v)
{
	m_aflValues[0] = v[0];
	m_aflValues[1] = v[1];
	m_aflValues[2] = v[2];
}

void vector::operator=(const glm::vec4& v)
{
	m_aflValues[0] = v[0];
	m_aflValues[1] = v[1];
	m_aflValues[2] = v[2];
}

vector::operator glm::vec3() const
{
	return glm::vec3(vec_x, vec_y, vec_z);
}

vector::operator glm::vec4() const
{
	return glm::vec4(vec_x, vec_y, vec_z, 0);
}

std::ostream& operator<<(std::ostream& s, const vector& v)
{
	s << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
	return s;
}
