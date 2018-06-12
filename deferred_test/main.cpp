#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/shader_program.h>
#include <glm/glm.hpp>

#include <gui/imgui.h>
#include "../graphics/gui/imgui_impl_sdl_gl3.h"

#undef main

const glm::mat4 mat_proj = glm::perspective(glm::radians(90.0f), 640.f / 360.f, 0.00001f, 1000.f);
const glm::mat4 mat_view = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -1.5f, -2.5f));
const glm::mat4 mat_trans = glm::mat4(1.0f);

static float deg = 0.0f;

void draw_model(const gfx::framebuffer& fb, gfx::model_id id) {
	gfx::material pMaterial = gpGfx->model_material(id);
	// Get the material's shader index
	int iShader = pMaterial.get_shader();
	if (iShader != -1) {
		gfx::shader_program* pShader = gpGfx->get_shader(iShader);
		pShader->set_int("fb_diffuse", fb.diffuse()->handle());
		pShader->set_int("fb_normal", fb.normal()->handle());
		gpGfx->bind_model(id);
		pShader->use();
		pShader->use_material(pMaterial);

		glm::mat4 proj = glm::perspective(glm::radians(120.f), 640.f / 360.f, 0.0f, 1000.0f);
		//glm::mat4 proj = glm::perspective(glm::radians(120.f), 1.0f, 0.0f, 1000.0f);
		//glm::mat4 proj = glm::ortho(0.0f, 512.f, 512.f, 0.0f, 0.0f, 1000.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-5.5f, -1.5f,-4.5f));
		//view = glm::rotate(view, glm::radians(180.f), glm::vec3(0, 0, 1));
		view = glm::rotate(view, glm::radians(deg), glm::vec3(0, 1, 0));
		glm::mat4 trans(1.0f);

		pShader->set_mat_view((void*)glm::value_ptr(view));
		pShader->set_mat_proj((void*)glm::value_ptr(proj));
		pShader->set_mat_trans((void*)glm::value_ptr(mat_trans));
		gpGfx->draw_model();
	}
}

int main() {
	gpGfx->init("engine deferred");

	gpGfx->load_default_shaders();

	auto mdl1 = gpGfx->load_model("data/models/csoldier.emf");
	auto mdl2 = gpGfx->load_model("data/models/wolf.emf");
	gfx::shared_fb fb = std::make_shared<gfx::framebuffer>(640, 360);
	ImVec2 uv0(0, 1); 
	ImVec2 uv1(1, 0);
	ImVec2 fbsiz(640, 360);
	while (1) {
		if (gpGfx->handle_events())
			break;

		deg += gpGfx->delta() * 45.f;
		if (deg > 360)
			deg = 0;
		//gpGfx->begin_frame();
		gpGfx->depth_test(false);
		// Render to G-Buffer
		fb->bind();
		gpGfx->clear_color(1.0, 0, 0);
		gpGfx->clear();
		gpGfx->wireframe(true);
		draw_model(*fb, mdl1);
		draw_model(*fb, mdl2);
		gpGfx->wireframe(false);
		fb->unbind();

		// Render normal buffer tex
		
		gpGfx->begin_frame();
		gpGfx->clear_color(0, 0, 1.0);
		gpGfx->clear();
		//gpGfx->depth_test(false);
		//gpGfx->draw_framebuffer(fb);
		if (ImGui::Begin("G-Buffers")) {
			int nShaders = gpGfx->shader_list().size();
			ImGui::InputInt("Shader count", &nShaders, 0);
			ImGui::Image((ImTextureID)fb->diffuse()->handle(), fbsiz);
			ImGui::Image((ImTextureID)fb->normal()->handle(), fbsiz);
		}
		ImGui::End();
		gpGfx->end_frame();
	}

	gpGfx->shutdown();
	return 0;
}
