#include "stdafx.h"
#include "game.h"
#include <gfx/gfx.h>
#include <gfx/material.h>
#include <gfx/shader_program.h>
#include "imgui_netgraph.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gui/imgui.h>

#define GBUF_WIDTH (640)
#define GBUF_HEIGHT (360)

game::game() : m_bPaused(true), m_model_cache({ 0 }) {
	m_proj = glm::perspective(glm::radians(90.f), (float)gpGfx->width() / (float)gpGfx->height(), 0.1f, 1000.0f);
	m_input.assign_camera(&m_camera);
	m_evhandler.assign_input_handler(&m_input);
	m_fb = std::make_shared<gfx::framebuffer>(GBUF_WIDTH, GBUF_HEIGHT);
}

void game::connect(const char * pszHostname, const char * pszUsername) {
	if (m_pNetClient)
		return;
	m_pNetClient = std::make_shared<net::client>(pszHostname, pszUsername);
	ASSERT(m_pNetClient);

	m_pNetClient->connect();
	//while (!m_pNetClient->connected()) {
	//	std::this_thread::sleep_for(std::chrono::seconds(1));
	//	m_pNetClient->attempt_connect();
	//}
	gpGfx->capture_mouse(true);
}

void game::connect(const sockaddr_in6 & addr, const char * pszUsername) {
	if (m_pNetClient)
		return;
}

void game::disconnect() {
	if (!m_pNetClient)
		return;
	m_pNetClient->disconnect();
	m_pNetClient.reset();
	gpGfx->capture_mouse(false);
}

bool game::tick() {
	std::vector<SDL_Event> events;
	gpGfx->get_events(events);
	
	for (auto& ev : events) {
		if (ev.type == SDL_QUIT)
			return false;
		m_evhandler.send_events(events);
		gpGfx->gui_send_event(ev);
	}

	if (!m_pNetClient)
		return true;

	// Send input
	std::vector<std::string> commands = m_input.update();
	auto& cmdbuf = m_pNetClient->get_command_buf();
	for (auto& cmd : commands) {
		cmdbuf.push_front(cmd);
	}
	m_pNetClient->push_client_updates();
	//ImGui::NetGraph(m_pNetClient->get_packet_stats());
	net::edict_t* edicts = m_pNetClient->get_edicts();

	gfx::shader_program* pPrevShader = nullptr;
	glm::mat4 mat_view = m_camera.get_rot();
	mat_view = glm::translate(mat_view, glm::vec3(m_camera.get_pos()));
	float flAspect = ((float)GBUF_WIDTH) / ((float)GBUF_HEIGHT);
	m_proj = glm::perspective(glm::radians(90.f), flAspect, 0.00001f, 1000.0f);

	m_fb->bind();
	gpGfx->blend(false);
	gpGfx->set_viewport(GBUF_WIDTH, GBUF_HEIGHT);
	gpGfx->clear_color(0, 0, 0);
	gpGfx->clear();
	//gpGfx->wireframe(true);

	for (size_t i = 1; i < net::max_edicts; i++) {
		if (edicts[i].active) {
			net::edict_t e = edicts[i];
			if (e.modelname[0] != '\0') {
				gfx::material mat;
				int iShader;
				gfx::shader_program* pShader;
				glm::mat4 mat_trans(1.0);

				if (m_model_cache[i] == 0) {
					m_model_cache[i] = gpGfx->load_model(e.modelname);
				}

				gfx::model_id mdl = m_model_cache[i];
				if (mdl == 0)
					continue;

				mat = gpGfx->model_material(mdl);
				iShader = mat.get_shader();
				if (iShader == -1)
					continue;
				pShader = gpGfx->get_shader(iShader);

				pShader->reload();
				if (pShader != pPrevShader)
					pShader->use();
				pShader->set_int("fb_diffuse", m_fb->diffuse()->handle());
				pShader->set_int("fb_normal", m_fb->normal()->handle());
				pShader->set_int("fb_worldpos", m_fb->worldpos()->handle());
				pShader->set_int("fb_specular", m_fb->specular()->handle());

				mat_trans = glm::translate(mat_trans, (glm::vec3)(e.iposition));
				mat_trans = mat_trans * glm::make_mat4(e.irotation);

				pPrevShader = pShader;
				pShader->set_mat_proj(glm::value_ptr(m_proj));
				pShader->set_mat_view(glm::value_ptr(mat_view));
				pShader->set_mat_trans(glm::value_ptr(mat_trans));
				gpGfx->bind_model(mdl);
				pShader->use_material(mat);
				gpGfx->depth_test(true);
				gpGfx->draw_model();
			}
			// Interpolate position
			float dt = gpGfx->delta();
			vec3 vel = e.ivelocity;
			//PRINT_DBG((dt * vel));
			edicts[i].iposition += dt * vel;
			// Interpolate rotation
			glm::vec3 angps = glm::make_vec3(e.angular_vel) * gpGfx->delta();
			glm::quat q(angps);
			glm::quat r(glm::make_mat4(e.irotation));
			r *= q;
			glm::mat4 m(r);
			memcpy(edicts[i].irotation, glm::value_ptr(m), 16 * sizeof(float));
		}
	}

	gpGfx->restore_viewport();
	m_fb->unbind();
	gpGfx->blend(true);
	gpGfx->depth_test(false);
	gpGfx->clear_color(-1);
	gpGfx->clear();
	gpGfx->draw_framebuffer(m_fb);
	gpGfx->draw_framebuffer(m_fb);

	ImVec2 fbsiz(640, 360);
	ImVec2 uv0(0, 1);
	ImVec2 uv1(0, 0);
	/*
	if (ImGui::Begin("G-Buffers")) {
		ImGui::BeginGroup();
		ImGui::Text("Albedo");
		ImGui::Image((ImTextureID)m_fb->diffuse()->handle(), fbsiz, uv0, uv1);
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text("Normal");
		ImGui::Image((ImTextureID)m_fb->normal()->handle(), fbsiz, uv0, uv1);
		ImGui::EndGroup();
		ImGui::BeginGroup();
		ImGui::Text("World position");
		ImGui::Image((ImTextureID)m_fb->worldpos()->handle(), fbsiz, uv0, uv1);
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text("Specular");
		ImGui::Image((ImTextureID)m_fb->specular()->handle(), fbsiz, uv0, uv1);
		ImGui::EndGroup();
	}
	ImGui::End();
	*/

	return true;
}
