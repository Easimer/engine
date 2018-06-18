#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gfx/pipeline/pipeline.h>
#include <gfx/pipeline/intake.h>
#include <gfx/pipeline/delivery.h>
#include <gfx/pipeline/intermediate.h>
#include <gfx/shader_program.h>
#include <gfx/framebuffer.h>

void gfx::pipeline::pipeline::set_intake(const intake& arg) {
	m_intake = arg;
}

void gfx::pipeline::pipeline::set_delivery(const delivery& arg) {
	m_delivery = arg;
}

void gfx::pipeline::pipeline::push_stage(const intermediate& arg) {
	m_stages.push_back(arg);
}

void gfx::pipeline::pipeline::begin() {
	if (m_intake) {
		PRINT_DBG(__func__);
		m_intake.begin();
	}
}

void gfx::pipeline::pipeline::draw(const draw_order& cmd) {
	if (m_intake) {
		PRINT_DBG(__func__);
		m_intake.draw(cmd);
	}
}

void gfx::pipeline::pipeline::finalize() {
	if (m_intake) {
		PRINT_DBG(__func__);
		m_intake.finalize();
		auto fb = m_intake.framebuffer();
		// Iterate through intermediate stages, if any
		for (gfx::pipeline::intermediate& stage : m_stages) {
			if (stage) {
				fb = stage.process(fb);
			}
		}
		m_delivery.process(fb);
	}
}

gfx::pipeline::intake::intake() {
	m_framebuffer = std::make_shared<gfx::framebuffer>();
}

void gfx::pipeline::intake::begin() {
	PRINT_DBG(__func__);
	m_framebuffer->unbind();

	m_framebuffer->bindw();
	gpGfx->clear();
}

void gfx::pipeline::intake::draw(const draw_order& cmd) {
	PRINT_DBG(__func__);
	auto material = gpGfx->model_material(cmd.model);
	gfx::shader_id shader_id = cmd.shader;
	if (shader_id != -1) {
		gfx::shader_program* shader = gpGfx->get_shader(shader_id);
		if (!shader)
			return;
		shader->reload();
		shader->use();
		shader->use_material(material);
		shader->set_int("fb_diffuse", m_framebuffer->diffuse()->handle());
		shader->set_int("fb_normal", m_framebuffer->normal()->handle());
		shader->set_int("fb_worldpos", m_framebuffer->worldpos()->handle());
		shader->set_mat_view((void*)glm::value_ptr(cmd.mat_view));
		shader->set_mat_proj((void*)glm::value_ptr(cmd.mat_proj));
		shader->set_mat_trans((void*)glm::value_ptr(cmd.mat_trans));
		gpGfx->bind_model(cmd.model);
		gpGfx->draw_model();
	}
}

void gfx::pipeline::intake::finalize() {
	PRINT_DBG(__func__);
	m_framebuffer->unbind();
}

void gfx::pipeline::delivery::process(gfx::shared_fb input) {
	PRINT_DBG(__func__);
	gpGfx->draw_framebuffer(input);
}

gfx::shared_fb gfx::pipeline::intermediate::process(gfx::shared_fb input) {
	PRINT_DBG(__func__);
	gfx::shared_fb ret = std::make_shared<gfx::framebuffer>();
	input->bindr();
	ret->bindw();

	ret->unbind();

	return ret;
}
