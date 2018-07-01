#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/shader_program.h>
#include <glm/glm.hpp>

#include <gui/imgui.h>
#include "../graphics/gui/imgui_impl_sdl_gl3.h"

#include <gfx/pipeline/pipeline.h>
#include <glm/gtc/random.hpp>
#include <array>

#include <iostream>
#include <fstream>
#include <efs/efs.h>

#undef main

const glm::mat4 mat_proj = glm::perspective(glm::radians(90.0f), 640.f / 360.f, 0.00001f, 1000.f);
const glm::mat4 mat_view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.75f, -0.75f));
const glm::mat4 mat_trans = glm::mat4(1.0f);

static float deg = 0.0f;

void draw_model(const gfx::framebuffer& fb, gfx::model_id id) {
	gfx::material pMaterial = gpGfx->model_material(id);
	// Get the material's shader index
	auto shader = pMaterial.get_shader();
	if (shader) {
		shader->reload();
		shader->use();
		shader->set_int("fb_diffuse", fb.diffuse()->handle());
		shader->set_int("fb_normal", fb.normal()->handle());
		shader->set_int("fb_worldpos", fb.worldpos()->handle());
		gpGfx->bind_model(id);
		//pShader->use_material(pMaterial);
		pMaterial.use();
		//view = glm::rotate(view, glm::radians(180.f), glm::vec3(0, 0, 1));
		glm::mat4 view = glm::rotate(mat_view, glm::radians(deg), glm::vec3(0, 1, 0));
		glm::mat4 trans(1.0f);

		shader->set_mat_view((void*)glm::value_ptr(view));
		shader->set_mat_proj((void*)glm::value_ptr(mat_proj));
		shader->set_mat_trans((void*)glm::value_ptr(mat_trans));
		gpGfx->draw_model();
	}
}

int main() {
	gpGfx->init("engine deferred", 1600, 900);

	gpGfx->load_default_shaders();

	auto mdl1 = gpGfx->load_model("data/models/csoldier.emf");
	auto mdl2 = gpGfx->load_model("data/models/wolf.emf");
	
	gfx::pipeline::pipeline pipeline("data/shaders/pipeline/default_pipeline.txt");

	std::array<gfx::shader_light, 4> lights;

	float lposstep = glm::golden_ratio<float>() * 64.0f;
	float nextdeg = 0.0;

	for (size_t i = 0; i < 4; i++) {
		lights[i].color.r = glm::linearRand(0.0, 1.0);
		lights[i].color.g = glm::linearRand(0.0, 1.0);
		lights[i].color.b = glm::linearRand(0.0, 1.0);
		lights[i].color.a = 4.0;

		float x = 4.0f * glm::cos(nextdeg);
		float z = 4.0f * glm::sin(nextdeg);
		lights[i].pos = vec3(x, 0.5f, z);
		nextdeg += lposstep;
	}

	while (1) {
		
		if (gpGfx->handle_events())
			break;

		deg += gpGfx->delta() * glm::radians(45.f);
		if (deg > glm::radians(360.0f))
			deg = 0;

		auto mat1 = gpGfx->model_material(mdl1);
		auto mat2 = gpGfx->model_material(mdl2);
		auto shader1 = mat1.get_shader();
		auto shader2 = mat1.get_shader();

		glm::mat4 view = glm::rotate(mat_view, deg, glm::vec3(0, 1, 0));

		gfx::pipeline::draw_order cmd1 = { mdl1, shader1, mat_trans, view, mat_proj };
		
		gfx::debug_marker mrk("Pipeline");
		pipeline.begin();
		
		if (ImGui::Begin("##empty")) {
			float d = deg;
			ImGui::SliderFloat("Degrees", &d, 0, glm::radians(360.0f));

			ImGui::Separator();
			ImGui::Text("Pipeline state");
			auto state = pipeline.debug_info();
			for (const auto& stage : state) {
				bool ready = stage.ready;
				bool lit = stage.lit;
				ImGui::BeginGroup();
				ImGui::Text(stage.name.c_str());
				ImGui::Checkbox("Ready", &ready);
				ImGui::Checkbox("Lit", &lit);
				ImGui::Text("State: %s", gfx::pipeline::strerror(stage.state));
				ImVec2 s(128, 72);
				ImGui::Image((ImTextureID)stage.texture_color, s, ImVec2(0, 1), ImVec2(1, 0)); ImGui::SameLine();
				ImGui::Image((ImTextureID)stage.texture_normal, s, ImVec2(0, 1), ImVec2(1, 0)); ImGui::NewLine();
				ImGui::Image((ImTextureID)stage.texture_worldpos, s, ImVec2(0, 1), ImVec2(1, 0)); ImGui::SameLine();
				ImGui::Image((ImTextureID)stage.texture_specular, s, ImVec2(0, 1), ImVec2(1, 0)); ImGui::NewLine();
				ImGui::Image((ImTextureID)stage.texture_selfillum, s, ImVec2(0, 1), ImVec2(1, 0)); ImGui::NewLine();
				ImGui::EndGroup();
				ImGui::SameLine();
			}
		}
		ImGui::End();
		pipeline.draw(cmd1);

		for (size_t i = 0; i < 4; i++) {
			pipeline.light(i, lights[i]);
		}

		pipeline.finalize((void*)glm::value_ptr(mat_view));
	}

	gpGfx->shutdown();
	return 0;
}
