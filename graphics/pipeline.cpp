#include "stdafx.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gfx/pipeline/pipeline.h>
#include <gfx/pipeline/stage.h>
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
		m_intermediate = s;
	}
}

void gfx::pipeline::pipeline::set_intake(const intake& arg) {
	m_intake = arg;
}

void gfx::pipeline::pipeline::set_delivery(const delivery& arg) {
	m_delivery = arg;
}

void gfx::pipeline::pipeline::set_intermediate(const intermediate& arg) {
	m_intermediate = arg;
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

void gfx::pipeline::pipeline::light(size_t iIndex, const gfx::shader_light& l) {
	ASSERT(iIndex < lights_limit);
	if (iIndex < lights_limit) {
		m_lights[iIndex] = l;
	}
}

void gfx::pipeline::pipeline::finalize(void* pMatProj) {
	if (m_intake) {
		{
			gfx::debug_marker mrk("Intake");
			m_intake.finalize();
		}
		auto fb = m_intake.framebuffer();
		if (m_intermediate) {
			if (m_intermediate.lit()) {
				fb = m_intermediate.process(m_lights, fb);
			} else {
				fb = m_intermediate.process(fb);
			}
		}
		m_delivery.process(fb, pMatProj);
	}
}

std::vector<gfx::pipeline::stage_info> gfx::pipeline::pipeline::debug_info() {
	std::vector<gfx::pipeline::stage_info> ret;

	gfx::pipeline::stage_info inf;
	inf.name = "(default)";
	inf.lit = false;
	inf.ready = (bool)m_intake;
	inf.index = 0;
	inf.texture_color = m_intake.color();
	inf.texture_normal = m_intake.normal();
	inf.texture_worldpos = m_intake.worldpos();
	inf.texture_specular = m_intake.specular();
	inf.texture_selfillum = m_intake.selfillum();
	inf.type = stage_intake;
	inf.state = m_intake.state();
	ret.push_back(inf);

	inf.ready = (bool)m_intermediate;
	inf.index = 1;
	inf.lit = m_intermediate.lit();
	inf.texture_color = m_intermediate.color();
	inf.texture_normal = m_intermediate.normal();
	inf.texture_worldpos = m_intermediate.worldpos();
	inf.texture_specular = m_intermediate.specular();
	inf.texture_selfillum = m_intermediate.selfillum();
	inf.type = stage_intermediate;
	inf.state = m_intermediate.state();
	ret.push_back(inf);

	inf.ready = (bool)m_delivery;
	inf.index = 2;
	inf.lit = false;
	inf.texture_color = m_delivery.color();
	inf.texture_normal = m_delivery.normal();
	inf.texture_worldpos = m_delivery.worldpos();
	inf.texture_specular = m_delivery.specular();
	inf.type = stage_delivery;
	inf.state = m_delivery.state();
	ret.push_back(inf);

	return ret;
}

gfx::pipeline::intake::intake() {
	m_framebuffer = std::make_shared<gfx::framebuffer>();
	m_ready = true;
}

void gfx::pipeline::intake::begin() {
	m_framebuffer->unbind();

	gpGfx->begin_frame();
	//PRINT_DBG("Intake");
	m_framebuffer->bindw();
	gpGfx->clear_color(0, 0, 0);
	gpGfx->clear();
	gpGfx->blend(false);
	gpGfx->depth_test(true);
}

void gfx::pipeline::intake::draw(const draw_order& cmd) {
	auto material = gpGfx->model_material(cmd.model());
	gfx::shader_id shader_id = cmd.shader();
	if (shader_id != -1) {
		gfx::shader_program* shader = gpGfx->get_shader(shader_id);
		if (!shader)
			return;
		shader->reload();
		if (!shader->use()) {
			return;
		}
		shader->use_material(material);
		shader->set_int("fb_diffuse", m_framebuffer->diffuse()->handle());
		shader->set_int("fb_normal", m_framebuffer->normal()->handle());
		shader->set_int("fb_worldpos", m_framebuffer->worldpos()->handle());
		shader->set_int("fb_specular", m_framebuffer->specular()->handle());
		shader->set_int("fb_selfillum", m_framebuffer->selfillum()->handle());
		shader->set_mat_view((void*)glm::value_ptr(cmd.mat_view()));
		shader->set_mat_proj((void*)glm::value_ptr(cmd.mat_proj()));
		shader->set_mat_trans((void*)glm::value_ptr(cmd.mat_trans()));
		gpGfx->bind_model(cmd.model());
		gpGfx->draw_model();
	}
}

void gfx::pipeline::intake::finalize() {
	gpGfx->clear_color();
}

gfx::pipeline::delivery::delivery() : stage(true) {
	m_shader = gpGfx->get_shader(gpGfx->get_shader_program_index("delivery_fb"));
}

void gfx::pipeline::delivery::process(gfx::shared_fb input, void* pMatProj) {
	gfx::debug_marker mrk("Delivery");
	ASSERT(m_shader);
	if (!m_shader)
		return;
	//PRINT_DBG("Delivery");
	m_shader->reload();
	if (!m_shader->use()) {
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//PRINT_DBG("Bound framebuffer (draw): 0");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input->diffuse()->handle());
	m_shader->set_int("tex_diffuse", 0);
	m_shader->set_mat_view(pMatProj);

	gpGfx->clear();

	glBindVertexArray(gpGfx->quad());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//input->unbind();
	gpGfx->end_frame();
}

gfx::pipeline::intermediate::intermediate(const std::string& filename) : stage(false), m_lit(false) {
	mdlc::qc stage_qc;
	stage_qc = mdlc::qc(std::ifstream(filename));
	if (stage_qc.count("name") && stage_qc.count("shader")) {
		m_name = stage_qc.at<std::string>("name");

		size_t iShader = gpGfx->get_shader_program_index(stage_qc.at<std::string>("shader"));
		m_shader = gpGfx->get_shader(iShader);

		m_framebuffer1 = std::make_shared<gfx::framebuffer>();
		m_framebuffer2 = std::make_shared<gfx::framebuffer>();
		m_ready = true;
	} else {
		ASSERT(0);
	}
	m_lit = stage_qc.at<bool, false>("lit");
}

gfx::shared_fb gfx::pipeline::intermediate::process(gfx::shared_fb input) {
	gfx::debug_marker mrk("Intermediate (unlit path)");
	gfx::shared_fb output = m_framebuffer1;
	if (input == m_framebuffer1)
		output = m_framebuffer2;
	gpGfx->blend(true);
	gpGfx->depth_test(false);
	//gpGfx->blend(false);
	//gpGfx->depth_test(false);
	//input->bindr();
	output->bind();
	//gpGfx->clear_color(0.0, 0.0, 0.0);
	//gpGfx->clear();
	m_shader->reload();
	if (!m_shader->use()) {
		m_state = stage_err_bad_shader;
		return output;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input->diffuse()->handle());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, input->normal()->handle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, input->worldpos()->handle());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, input->specular()->handle());
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, input->selfillum()->handle());
	m_shader->set_int("tex_diffuse", 0);
	m_shader->set_int("tex_normal", 1);
	m_shader->set_int("tex_worldpos", 2);
	m_shader->set_int("tex_specular", 3);
	m_shader->set_int("tex_selfillum", 4);
	m_shader->set_int("fb_diffuse", output->diffuse()->handle());
	m_shader->set_int("fb_normal", output->normal()->handle());
	m_shader->set_int("fb_worldpos", output->worldpos()->handle());
	m_shader->set_int("fb_specular", output->specular()->handle());
	m_shader->set_int("fb_selfillum", output->selfillum()->handle());

	glBindVertexArray(gpGfx->quad());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//gpGfx->blend(true);
	//gpGfx->depth_test(false);
	m_state = stage_err_ok;
	return output;
}

gfx::shared_fb gfx::pipeline::intermediate::process(const std::array<gfx::shader_light, lights_limit>& l, gfx::shared_fb input) {
	gfx::debug_marker mrk("Intermediate (lit path)");
	gfx::shared_fb output = m_framebuffer1;
	if (input == m_framebuffer1)
		output = m_framebuffer2;
	//PRINT_DBG("Intermediate (lit path)");
	gpGfx->blend(true);
	gpGfx->depth_test(false);
	//gpGfx->blend(false);
	//gpGfx->depth_test(false);
	//input->bindr();
	output->bind();
	gpGfx->clear_color(0.0, 0.0, 0.0);
	gpGfx->clear();
	m_shader->reload();
	if (!m_shader->use()) {
		m_state = stage_err_bad_shader;
		return output;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input->diffuse()->handle());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, input->normal()->handle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, input->worldpos()->handle());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, input->specular()->handle());
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, input->selfillum()->handle());
	m_shader->set_int("tex_diffuse", 0);
	m_shader->set_int("tex_normal", 1);
	m_shader->set_int("tex_worldpos", 2);
	m_shader->set_int("tex_specular", 3);
	m_shader->set_int("tex_selfillum", 4);
	//m_shader->set_int("fb_diffuse", output->diffuse()->handle());
	//m_shader->set_int("fb_normal", output->normal()->handle());
	//m_shader->set_int("fb_worldpos", output->worldpos()->handle());
	//m_shader->set_int("fb_specular", output->specular()->handle());
	m_shader->set_vec3("light_pos", l[0].pos);
	m_shader->set_vec4("light_color", &(l[0].color.r));

	glBindVertexArray(gpGfx->quad());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//gpGfx->blend(true);
	//gpGfx->depth_test(false);
	m_state = stage_err_ok;
	return output;
}