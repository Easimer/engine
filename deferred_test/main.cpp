#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/shader_program.h>
#include <glm/glm.hpp>

#include <gui/imgui.h>
#include "../graphics/gui/imgui_impl_sdl_gl3.h"

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
	gfx::shared_fb fb = std::make_shared<gfx::framebuffer>(640, 360);
	ImVec2 uv0(0, 1); 
	ImVec2 uv1(1, 0);
	ImVec2 fbsiz(640, 360);
	gpGfx->depth_test(true);
	const int base_width = 640;
	const int base_height = 360;
	float flFBLambdaCur = 1.0f;
	int cur_width = base_width;
	int cur_height = base_height;
	while (1) {
		float flFBLambda = flFBLambdaCur;
		if (gpGfx->handle_events())
			break;

		deg += gpGfx->delta() * 45.f;
		if (deg > 360)
			deg = 0;
		//gpGfx->begin_frame();
		gpGfx->depth_test(true);

		//
		// NOTE:
		// DO NOT FORGET TO DISABLE BLEND IF YOU DO DEFERRED RENDERING
		//

		gpGfx->blend(false);
		// Render to G-Buffer
		fb->bind();	
		gpGfx->set_viewport(cur_width, cur_height);
		gpGfx->clear_color(0, 0, 0);
		gpGfx->clear();
		//gpGfx->wireframe(true);
		draw_model(*fb, mdl1);
		draw_model(*fb, mdl2);
		//gpGfx->wireframe(false);
		gpGfx->restore_viewport();
		fb->unbind();
		gpGfx->blend(true);
		// Render normal buffer tex
		
		gpGfx->begin_frame();
		gpGfx->clear_color(-1);
		gpGfx->clear();
		gpGfx->depth_test(false);
		gpGfx->draw_framebuffer(fb);
		if (ImGui::Begin("G-Buffers")) {
			ImGui::BeginGroup();
			ImGui::Text("Albedo");
			ImGui::Image((ImTextureID)fb->diffuse()->handle(), fbsiz, uv0, uv1);
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text("Normal");
			ImGui::Image((ImTextureID)fb->normal()->handle(), fbsiz, uv0, uv1);
			ImGui::EndGroup();
			ImGui::BeginGroup();
			ImGui::Text("World position");
			ImGui::Image((ImTextureID)fb->worldpos()->handle(), fbsiz, uv0, uv1);
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text("Specular");
			ImGui::Image((ImTextureID)fb->specular()->handle(), fbsiz, uv0, uv1);
			ImGui::EndGroup();
			ImGui::SliderFloat("##empty", &flFBLambda, 0.0f, 16.0f);
			ImGui::Text("Internal resolution: %fx%f\n", base_width * flFBLambdaCur, base_width * flFBLambdaCur);
		}
		ImGui::End();
		gpGfx->end_frame();
		if (flFBLambda != flFBLambdaCur) {
			fb = std::make_shared<gfx::framebuffer>(base_width * flFBLambda, base_height * flFBLambda);
			flFBLambdaCur = flFBLambda;
			cur_width = flFBLambdaCur * base_width;
			cur_height = flFBLambdaCur * base_height;
		}
	}

	gpGfx->shutdown();
	return 0;
}
