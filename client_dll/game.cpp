#include "stdafx.h"
#include "game.h"
#include <gfx/gfx.h>
#include <gfx/material.h>
#include <gfx/shader_program.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

game::game() : m_bPaused(true), m_model_cache({ 0 }) {
	m_proj = glm::perspective(glm::radians(90.f), (float)gpGfx->width() / (float)gpGfx->height(), 0.1f, 1000.0f);
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
}

bool game::tick() {
	std::vector<SDL_Event> events;
	gpGfx->get_events(events);
	
	for (auto& ev : events)
		gpGfx->gui_send_event(ev);

	if (!m_pNetClient)
		return true;

	glm::mat4 mat_view = m_camera.get_rot();
	mat_view = glm::translate(mat_view, glm::vec3(m_camera.get_pos()));
	
	net::edict_t* edicts = m_pNetClient->get_edicts();
	for (size_t i = 1; i < net::max_edicts; i++) {
		if (edicts[i].active) {
			if (edicts[i].modelname[0] != '\0') {
				gfx::material mat;
				int iShader;
				gfx::shader_program* pShader;
				glm::mat4 mat_trans(1.0);

				if (m_model_cache[i] == 0) {
					m_model_cache[i] = gpGfx->load_model(edicts[i].modelname);
				}

				gfx::model_id mdl = m_model_cache[i];
				if (mdl == 0)
					continue;

				mat = gpGfx->model_material(mdl);
				iShader = mat.get_shader();
				if (iShader == -1)
					continue;
				pShader = gpGfx->get_shader(iShader);

				/*std::cout << "=========" << std::endl;
				for (size_t y = 0; y < 4; y++) {
					for (size_t x = 0; x < 4; x++) {
						std::cout << edicts[i].rotation[4 * y + x] << '\t';
					}
					std::cout << std::endl;
				}
				std::cout << "=========" << std::endl;*/

				mat_trans = glm::translate(mat_trans, (glm::vec3)edicts->iposition);
				mat_trans = mat_trans * glm::make_mat4(edicts[i].irotation);

				pShader->use();
				pShader->set_mat_proj(glm::value_ptr(m_proj));
				pShader->set_mat_view(glm::value_ptr(mat_view));
				pShader->set_mat_trans(glm::value_ptr(mat_trans));
				gpGfx->bind_model(mdl);
				pShader->use_material(mat);
				gpGfx->draw_model();
			}
			// Interpolate position
			float dt = gpGfx->delta();
			vec3 vel = edicts[i].ivelocity;
			float dx = vel.x() * dt;
			float dy = vel.y() * dt;
			float dz = vel.z() * dt;
			//PRINT_DBG((dt * vel));
			edicts[i].iposition[0] += dx;
			if(edicts[i].iacceleration[1] < 0)
				edicts[i].iposition[1] += dy / 2;
			else
				edicts[i].iposition[1] += dy;
			edicts[i].iposition[2] += dz;
			// Interpolate rotation
			glm::vec3 angps = glm::make_vec3(edicts[i].angular_vel) * gpGfx->delta();
			glm::quat q(angps);
			glm::quat r(glm::make_mat4(edicts[i].irotation));
			r *= q;
			glm::mat4 m(r);
			memcpy(edicts[i].irotation, glm::value_ptr(m), 16 * sizeof(float));
		}
	}

	return true;
}
