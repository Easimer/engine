#include "stdafx.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gfx/pipeline/pipeline.h>
#include <gfx/pipeline/intake.h>
#include <gfx/pipeline/delivery.h>
#include <gfx/pipeline/intermediate.h>
#include <gfx/shader_program.h>
#include <gfx/framebuffer.h>
#include <qc.h>
#include "../graphics/glad/glad.h"

gfx::pipeline::pipeline::pipeline(const std::string & filename) {
	std::ifstream f(filename);
	if (!f) return;
	std::string line;
	PRINT_DBG("Loading pipeline " << filename);
	while (std::getline(f, line)) {
		gfx::pipeline::intermediate s(line);
		PRINT_DBG("Loaded stage: " << s.name());
		m_stages.push_back(s);
	}
}

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
		m_intake.begin();
	}
}

void gfx::pipeline::pipeline::draw(const draw_order& cmd) {
	if (m_intake) {
		m_intake.draw(cmd);
	}
}

void gfx::pipeline::pipeline::finalize() {
	if (m_intake) {
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
	m_ready = true;
}

void gfx::pipeline::intake::begin() {
	m_framebuffer->unbind();

	gpGfx->begin_frame();
	m_framebuffer->bindw();
	gpGfx->clear_color(0, 0, 0);
	gpGfx->clear();
	gpGfx->blend(false);
	gpGfx->depth_test(true);
}

void gfx::pipeline::intake::draw(const draw_order& cmd) {
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
	m_framebuffer->unbind();
	gpGfx->clear_color();
	gpGfx->blend(true);
	gpGfx->depth_test(false);
}

void gfx::pipeline::delivery::process(gfx::shared_fb input) {
	gpGfx->clear();
	gpGfx->draw_framebuffer(input);
	gpGfx->end_frame();
}

gfx::pipeline::intermediate::intermediate(const std::string& filename) : stage(false) {
	mdlc::qc stage_qc;
	stage_qc = mdlc::qc(std::ifstream(filename));
	if (stage_qc.count("name") && stage_qc.count("shader")) {
		m_name = stage_qc.at<std::string>("name");

		size_t iShader = gpGfx->get_shader_program_index(stage_qc.at<std::string>("shader"));
		m_shader = gpGfx->get_shader(iShader);

		m_framebuffer = std::make_shared<gfx::framebuffer>();
		m_ready = true;
	} else {
		ASSERT(0);
	}
}

gfx::shared_fb gfx::pipeline::intermediate::process(gfx::shared_fb input) {
	input->bindr();
	m_framebuffer->bindw();
	gpGfx->clear();
	m_shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input->diffuse()->handle());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, input->normal()->handle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, input->worldpos()->handle());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, input->specular()->handle());
	m_shader->set_int("tex_diffuse", 0);
	m_shader->set_int("tex_normal", 1);
	m_shader->set_int("tex_worldpos", 2);
	m_shader->set_int("tex_specular", 3);

	

	glBindVertexArray(gpGfx->quad());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	m_framebuffer->unbind();

	return m_framebuffer;
}
