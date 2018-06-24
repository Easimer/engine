#pragma once

#include <memory>
#include <array>
#include <gfx/pipeline/intake.h>
#include <gfx/pipeline/intermediate.h>
#include <gfx/pipeline/delivery.h>
#include <gfx/light.h>
#include <glm/glm.hpp>

namespace gfx::pipeline {

class idraw_order {
public:
	virtual gfx::model_id model() const noexcept = 0;
	virtual gfx::shared_shader_program shader() const noexcept = 0;
	virtual const glm::mat4& mat_trans() const noexcept = 0;
	virtual const glm::mat4& mat_view() const noexcept = 0;
	virtual const glm::mat4& mat_proj() const noexcept = 0;
};

class draw_order : public idraw_order {
public:
	draw_order(gfx::model_id model, gfx::shared_shader_program shader,
		const glm::mat4& trans, const glm::mat4& view, const glm::mat4& proj) :
		m_model(model), m_shader(shader),
		m_mat_trans(trans), m_mat_view(view), m_mat_proj(proj) {}
	virtual gfx::model_id model() const noexcept final { return m_model; }
	virtual gfx::shared_shader_program shader() const noexcept final { return m_shader; };
	virtual const glm::mat4& mat_trans() const noexcept final { return m_mat_trans; }
	virtual const glm::mat4& mat_view() const noexcept final { return m_mat_view; }
	virtual const glm::mat4& mat_proj() const noexcept final { return m_mat_proj; }
private:
	gfx::model_id m_model;
	gfx::shared_shader_program m_shader;
	//size_t material; // TODO: ref to mat obj can't be queried through gpGfx
	glm::mat4 m_mat_trans;
	glm::mat4 m_mat_view;
	glm::mat4 m_mat_proj;
};

class draw_order_matref : public idraw_order {
public:
	draw_order_matref(gfx::model_id model, gfx::shared_shader_program shader,
		const glm::mat4& trans, const glm::mat4& view, const glm::mat4& proj) :
		m_model(model), m_shader(shader),
		m_mat_trans(trans), m_mat_view(view), m_mat_proj(proj) {
	}
	virtual gfx::model_id model() const noexcept final { return m_model; }
	virtual gfx::shared_shader_program shader() const noexcept final { return m_shader; };
	virtual const glm::mat4& mat_trans() const noexcept final { return m_mat_trans; }
	virtual const glm::mat4& mat_view() const noexcept final { return m_mat_view; }
	virtual const glm::mat4& mat_proj() const noexcept final { return m_mat_proj; }
private:
	gfx::model_id m_model;
	gfx::shared_shader_program m_shader;
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

	void begin(bool bGUI = true);
	void draw(const idraw_order&);
	void light(size_t iIndex, const gfx::shader_light&);
	void finalize(void* pMatView);

	void set_view_matrix(const glm::mat4& mat) noexcept {
		m_mat_view = mat;
	}

	std::vector<stage_info> debug_info();

	const intake& intake() const noexcept { return m_intake; }
	const intermediate& intermediate() const noexcept { return m_intermediate; }
	const delivery& delivery() const noexcept { return m_delivery; }
private:
	gfx::pipeline::intake m_intake;
	gfx::pipeline::delivery m_delivery;
	gfx::pipeline::intermediate m_intermediate;
	//std::vector<intermediate> m_stages;
	std::array<gfx::shader_light, 4> m_lights;
	bool m_bGUI;
	glm::mat4 m_mat_view;
};

}
