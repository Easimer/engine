#include "stdafx.h"
#include "prop_common.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

class c_prop_static : public c_base_prop {
public:
	DEC_CLASS(prop_static, c_base_prop);

	void precache();
	void spawn();

	void set_abspos(const vec3& v) override {}
	void set_rotation(const vec3& v) override {}
	void set_scale(float flScale) override {}

	virtual bool is_static() const override {
		return true;
	}

	virtual glm::mat4 get_transform_matrix() const override {
		return m_matTrans;
	}

private:
	glm::mat4 m_matTrans;
};

REGISTER_ENTITY(c_prop_static, prop_static);

void c_prop_static::precache()
{
	BaseClass::precache();
}

void c_prop_static::spawn()
{
	BaseClass::spawn();
	precache();

	m_matTrans = glm::scale(glm::mat4(1.0), glm::vec3(m_flScale, m_flScale, m_flScale));
	m_matTrans = glm::eulerAngleXYZ(m_vecRot[0], m_vecRot[1], m_vecRot[2]) * m_matTrans;
	m_matTrans = glm::translate(m_matTrans,(glm::vec3)m_vecPos);
}
