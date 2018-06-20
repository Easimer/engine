#pragma once

#include <memory>
#include <array>
#include <gfx/pipeline/intake.h>
#include <gfx/pipeline/intermediate.h>
#include <gfx/pipeline/delivery.h>
#include <gfx/light.h>

namespace gfx::pipeline {

class idraw_order {
public:
	virtual gfx::model_id model() const noexcept = 0;
	virtual size_t shader() const noexcept = 0;
	virtual const glm::mat4& mat_trans() const noexcept = 0;
	virtual const glm::mat4& mat_view() const noexcept = 0;
	virtual const glm::mat4& mat_proj() const noexcept = 0;
};

class draw_order : public idraw_order {
public:
	draw_order(gfx::model_id model, size_t shader,
		const glm::mat4& trans, const glm::mat4& view, const glm::mat4& proj) :
		m_model(model), m_shader(shader),
		m_mat_trans(trans), m_mat_view(view), m_mat_proj(proj) {}
	virtual gfx::model_id model() const noexcept final { return m_model; }
	virtual size_t shader() const noexcept final { return m_shader; };
	virtual const glm::mat4& mat_trans() const noexcept final { return m_mat_trans; }
	virtual const glm::mat4& mat_view() const noexcept final { return m_mat_view; }
	virtual const glm::mat4& mat_proj() const noexcept final { return m_mat_proj; }
private:
	gfx::model_id m_model;
	size_t m_shader;
	//size_t material; // TODO: ref to mat obj can't be queried through gpGfx
	glm::mat4 m_mat_trans;
	glm::mat4 m_mat_view;
	glm::mat4 m_mat_proj;
};

class draw_order_matref : public idraw_order {
public:
	draw_order_matref(gfx::model_id model, size_t shader,
		const glm::mat4& trans, const glm::mat4& view, const glm::mat4& proj) :
		m_model(model), m_shader(shader),
		m_mat_trans(trans), m_mat_view(view), m_mat_proj(proj) {
	}
	virtual gfx::model_id model() const noexcept final { return m_model; }
	virtual size_t shader() const noexcept final { return m_shader; };
	virtual const glm::mat4& mat_trans() const noexcept final { return m_mat_trans; }
	virtual const glm::mat4& mat_view() const noexcept final { return m_mat_view; }
	virtual const glm::mat4& mat_proj() const noexcept final { return m_mat_proj; }
private:
	gfx::model_id m_model;
	size_t m_shader;
	//size_t material; // TODO: ref to mat obj can't be queried through gpGfx
	const glm::mat4& m_mat_trans;
	const glm::mat4& m_mat_view;
	const glm::mat4& m_mat_proj;
};

class pipeline {
public:
	pipeline(const std::string& filename);
	void set_intake(const intake&);
	void set_delivery(const delivery&);
	void set_intermediate(const intermediate&);

	void begin();
	void draw(const draw_order&);
	void light(size_t iIndex, const gfx::shader_light&);
	void finalize(void* pMatProj);

	std::vector<stage_info> debug_info();
private:
	intake m_intake;
	delivery m_delivery;
	intermediate m_intermediate;
	//std::vector<intermediate> m_stages;
	std::array<gfx::shader_light, 4> m_lights;
};

}
