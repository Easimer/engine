#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/model.h>
#include <gfx/shader_program.h>
#include "igfx.h"
#include "event_handler.h"
#include <gfx/window_register.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

void igfx::draw_models(const std::vector<drawcmd_t>& drawcmds)
{
	m_drawcmds.begin_write();
	for (const auto& dc : drawcmds) {
		m_drawcmds.write(dc);
	}
	m_drawcmds.end_write();
}

uint32_t igfx::load_model(const std::string& filename)
{
	unsigned timeout = 1000;
	m_request_load.lock();
	m_load_requests.push_back({ gfx_load_type::model, filename});
	m_request_wait = -1;
	m_request_load.unlock();
	while (m_request_wait == -1) {
		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1));
		timeout--;
		if (timeout == 0) {
			PRINT_ERR("igfx(load_model)[LOGIC]: !!! timed out on " << filename);
			return 0;
		}
	}
	return (uint32_t)m_request_wait;
}

size_t igfx::load_shader(const std::string & filename)
{
	unsigned timeout = 1000;
	m_request_load.lock();
	m_load_requests.push_back({ gfx_load_type::shader, filename });
	m_request_wait = -1;
	m_request_load.unlock();
	while (m_request_wait == -1) {
		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1));
		timeout--;
		if (timeout == 0) {
			PRINT_ERR("igfx(load_shader)[LOGIC]: !!! timed out on " << filename);
		}
	}
	return (size_t)m_request_wait;
}

void igfx::begin_load()
{
	RESTRICT_THREAD_LOGIC;
	PRINT_ERR("igfx(begin_load)[LOGIC]: !!! stub function ");
}

void igfx::end_load()
{
	RESTRICT_THREAD_LOGIC;
	PRINT_ERR("igfx(end_load)[LOGIC]: !!! stub function ");
}

bool igfx::init(const char * szTitle, size_t width, size_t height, size_t glVersionMajor, size_t glVersionMinor)
{
	RESTRICT_THREAD_RENDERING;

	m_proj = glm::perspective(glm::radians(90.f), (float)width / (float)height, 0.1f, 1000.0f);

	bool bRet = gpGfx->init(szTitle, width, height, glVersionMajor, glVersionMinor);

	if (bRet) {
		gpGfx->capture_mouse(true);

		//gpGfx->add_window(gfx::gpWindowRegister->create("renderer_debug"));
		//gpGfx->add_window(gfx::gpWindowRegister->create("statistics"));
	}

	return bRet;
}

bool igfx::shutdown()
{
	RESTRICT_THREAD_RENDERING;
	return gpGfx->shutdown();
}

void igfx::update()
{
	RESTRICT_THREAD_RENDERING;

	// Check for resource load requests
	{
		std::lock_guard<std::mutex> wg(m_request_load);

		for (const auto& kv : m_load_requests) {
			switch (kv.first) {
			case gfx_load_type::model:
				m_request_wait = gpGfx->load_model(kv.second);
				break;
			case gfx_load_type::shader:
				m_request_wait = gpGfx->load_shader(kv.second);
				break;
			default:
				PRINT_ERR("igfx(check_load_requests)[RENDERING]: attempted to load unknown type of resource: " << kv.second);
				continue;
			}
			PRINT_ERR("igfx(check_load_requests)[RENDERING]: loaded " << kv.second);
		}
		m_load_requests.clear();
	}

	std::vector<SDL_Event> ev;
	gpGfx->get_events(ev);
	if (gpGlobals->bDevGUI) {
		// Pass the events to GUI
		for (auto& e : ev) {
			gpGfx->gui_send_event(e);
		}
	} else {
		// Pass the events to the event handler
		gpGlobals->pEventHandler->push_event(ev);
	}
}

void igfx::draw()
{
	RESTRICT_THREAD_RENDERING;
	drawcmd_t* pCommands;
	drawcmd_t* pCurCommand;
	size_t nCommands = 0;
	size_t iCommand = 0;
	size_t nAllVertices = 0;

	if (m_drawcmds.begin_read(&pCommands, &nCommands)) {
		gpGlobals->pStatistics->get_stat_u(ESTAT_C_RENDERER, "current draw commands") = nCommands;
		iCommand = nCommands;
		pCurCommand = pCommands;

		glm::mat4 mat_view = gpGlobals->pCamera->get_rot();
		mat_view = glm::translate(mat_view, glm::vec3(gpGlobals->pCamera->get_pos()));

		while (iCommand--) {
			gfx::material mat;
			int iShader;
			gfx::shader_program* pShader;
			glm::mat4 mat_trans(1.0);
			size_t nVertexCount = gpGfx->model_vertices(pCurCommand->iModelID);

			mat = gpGfx->model_material(pCurCommand->iModelID);
			iShader = mat.get_shader();
			if (iShader == -1) {
				PRINT_DBG("Shader failure");
				goto end_command;
				// Fallback to the first shader
				//iShader = 0;
			}

			pShader = gpGfx->get_shader(iShader);
			if (!pShader)
				goto end_command;

			pShader->use();
			pShader->set_mat_proj(glm::value_ptr(m_proj));
			pShader->set_mat_view(glm::value_ptr(mat_view));
			pShader->set_mat_trans(glm::value_ptr(mat_trans));
			pShader->set_bool("bDebugDrawNormalsOnly", false); // TODO
			gpGfx->bind_model(pCurCommand->iModelID);

			if (pCurCommand->bMatRotationIsTransformation) {
				mat_trans = pCurCommand->matRotation;
			} else {
				mat_trans = glm::translate(mat_trans, glm::vec3(pCurCommand->vecPosition[0], pCurCommand->vecPosition[1], pCurCommand->vecPosition[2]));
				mat_trans = mat_trans * pCurCommand->matRotation;
				mat_trans = glm::scale(mat_trans, glm::vec3(pCurCommand->flScale, pCurCommand->flScale, pCurCommand->flScale));
			}

			pShader->set_local_light(pCurCommand->light_local);
			pShader->set_global_light(pCurCommand->light_global);

			pShader->set_mat_trans(glm::value_ptr(mat_trans));

			pShader->use_material(mat);
			nAllVertices += nVertexCount;

			gpGfx->draw_model();

		end_command:
			pCurCommand++;
		}
		m_drawcmds.end_read();
	}
}
