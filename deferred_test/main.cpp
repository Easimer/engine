#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/shader_program.h>
#include <glm/glm.hpp>

#include <gui/imgui.h>
#include "../graphics/gui/imgui_impl_sdl_gl3.h"

#include <gfx/pipeline/pipeline.h>

#undef main

const glm::mat4 mat_proj = glm::perspective(glm::radians(90.0f), 640.f / 360.f, 0.00001f, 1000.f);
const glm::mat4 mat_view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.75f, -0.75f));
const glm::mat4 mat_trans = glm::mat4(1.0f);

static float deg = 0.0f;

void draw_model(const gfx::framebuffer& fb, gfx::model_id id) {
	gfx::material pMaterial = gpGfx->model_material(id);
	// Get the material's shader index
	int iShader = pMaterial.get_shader();
	if (iShader != -1) {
		gfx::shader_program* pShader = gpGfx->get_shader(iShader);
		pShader->reload();
		pShader->use();
		pShader->set_int("fb_diffuse", fb.diffuse()->handle());
		pShader->set_int("fb_normal", fb.normal()->handle());
		pShader->set_int("fb_worldpos", fb.worldpos()->handle());
		gpGfx->bind_model(id);
		pShader->use_material(pMaterial);
		//view = glm::rotate(view, glm::radians(180.f), glm::vec3(0, 0, 1));
		glm::mat4 view = glm::rotate(mat_view, glm::radians(deg), glm::vec3(0, 1, 0));
		glm::mat4 trans(1.0f);

		pShader->set_mat_view((void*)glm::value_ptr(view));
		pShader->set_mat_proj((void*)glm::value_ptr(mat_proj));
		pShader->set_mat_trans((void*)glm::value_ptr(mat_trans));
		gpGfx->draw_model();
	}
}

int main() {
	gpGfx->init("engine deferred", 1600, 900);

	gpGfx->load_default_shaders();

	auto mdl1 = gpGfx->load_model("data/models/csoldier.emf");
	auto mdl2 = gpGfx->load_model("data/models/wolf.emf");
	
	gfx::pipeline::pipeline pipeline("data/shaders/pipeline/default_pipeline.txt");
	
	while (1) {
		
		if (gpGfx->handle_events())
			break;

		deg += gpGfx->delta() * 45.f;
		if (deg > 360)
			deg = 0;

		glm::mat4 view = glm::rotate(mat_view, glm::radians(deg), glm::vec3(0, 1, 0));

		auto mat1 = gpGfx->model_material(mdl1);
		auto mat2 = gpGfx->model_material(mdl2);
		int shader1 = mat1.get_shader();
		int shader2 = mat1.get_shader();

		gfx::pipeline::draw_order cmd1 = { mdl1, shader1, mat_trans, view, mat_proj };
		gfx::pipeline::draw_order cmd2 = { mdl2, shader2, mat_trans, view, mat_proj };
		
		pipeline.begin();

		if (ImGui::Begin("Pipeline Test")) {

		}
		ImGui::End();
		pipeline.draw(cmd1);
		pipeline.draw(cmd2);
		pipeline.finalize();
		//draw_model(*fb, mdl1);
		//draw_model(*fb, mdl2);
	}

	gpGfx->shutdown();
	return 0;
}
