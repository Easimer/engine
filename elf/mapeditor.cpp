#include "stdafx.h"
#include <ifsys/ifsys.h>
#include "mapeditor.h"
#include <gui/imgui.h>
#include <gfx/gfx.h>
#include <gfx/shader_program.h>
#include <gfx/window_register.h>
#include "gui_objects.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace ImGui {

bool LevelSelector(char* pszPath, size_t nSize) {
	bool bRet = false;
	if (ImGui::Begin("Select a level")) {
		ImGui::ListBoxHeader("##empty");
#if defined(PLAT_WINDOWS)
		WIN32_FIND_DATA ffd;
		HANDLE hFile;
		const char* szPath = "data\\maps\\*";
		hFile = FindFirstFileA(szPath, &ffd);
		while (hFile != INVALID_HANDLE_VALUE) {
			if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				if (ImGui::Selectable(ffd.cFileName)) {
					strncpy(pszPath, ffd.cFileName, nSize);
				}
			}
			if (FindNextFileA(hFile, &ffd) == 0)
				break;
		}
		DWORD dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES) {
			PRINT_DBG(dwError);
		}
		FindClose(hFile);
#endif
		ImGui::ListBoxFooter();
		ImGui::InputText("Filename", pszPath, nSize, ImGuiInputTextFlags_ReadOnly);
		bRet = ImGui::Button("Select");
	}
	ImGui::End();
	return bRet;
}

}

extern "C" ENL_EXPORT void ifsys_fn(ifsys* is) {
	static mapeditor gMapEditor;
	is->connect("EngineLevelEditor0001", &gMapEditor);
	gMapEditor.set_ifsys(is);
}

void mapeditor::init() {
	m_bShutdown = false;
	m_bFreeCamera = false;
	m_flGameViewX = m_flGameViewY = 0;
	m_thread = m_pIfSys->make_thread();
	std::thread t([&]() {
		PRINT_DBG("Level editor init!");
		gpGfx->init("engine level editor", 1600, 900);
		PRINT_DBG("gfx init");
		// Load GUI icons
		load_icon(E_LED_TOOL_SELECT, "data/textures/gui/mapeditor/tool_select.png");
		load_icon(E_LED_TOOL_TERRAIN_RAISE, "data/textures/gui/mapeditor/tool_terrain_raise.png");
		load_icon(E_LED_TOOL_TERRAIN_LOWER, "data/textures/gui/mapeditor/tool_terrain_lower.png");

		gpGfx->load_default_shaders();

		auto pWndObjects = gfx::gpWindowRegister->create("GUIObjects");
		std::shared_ptr<gui_objects> pWndObjects2 = std::static_pointer_cast<gui_objects, gfx::window>(pWndObjects);
		pWndObjects2->set_is(m_pIfSys);
		gpGfx->add_window(pWndObjects);

		m_input.assign_camera(&m_camera);
		PRINT_DBG("leveleditor: entering loop");
		while (!m_bShutdown) {
			handle_events();
			gpGfx->begin_frame();
			draw_world();
			draw_gui();
			gpGfx->draw_windows();
			gpGfx->end_frame();
		}
		gpGfx->shutdown();
	});
	m_thread->swap(t);
}

bool mapeditor::shutdown() {
	m_bShutdown = true;
	PRINT_DBG("mapeditor::shutdown");
	if (m_thread->joinable()) {
		m_thread->join();
		return false;
	}
	return true;
}

void mapeditor::handle_events() {
	std::vector<SDL_Event> ev;
	gpGfx->get_events(ev);
	float delta = gpGfx->delta();
	m_camera.update(delta);
	m_input.update();
	auto& io = ImGui::GetIO();
	for (auto& e : ev) {
		switch (e.type) {
		case SDL_KEYDOWN:
			//if (io.WantCaptureKeyboard)
			//	gpGfx->gui_send_event(e);
			break;
		case SDL_KEYUP:
			//if(io.WantCaptureKeyboard)
			//	gpGfx->gui_send_event(e);
			//else
			{
				if (e.key.keysym.sym == SDLK_z) {
					m_bFreeCamera = !m_bFreeCamera;
					gpGfx->capture_mouse(m_bFreeCamera);
				}
			}
			break;
		case SDL_QUIT:
			m_bShutdown = true;
			break;
		case SDL_MOUSEMOTION:
			if(m_bFreeCamera)
				m_input.mouse_motion(e.motion.xrel, e.motion.yrel);
			//else
			//gpGfx->gui_send_event(e);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (!m_bFreeCamera && io.WantCaptureMouse) {
				PRINT_DBG("Sending MB event: " << m_bFreeCamera << "&" << io.WantCaptureMouse);
				//gpGfx->gui_send_event(e);
			}
			break;
		default:
			
			break;
		}
	}
}

void mapeditor::draw_gui() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "Ctrl-N")) {
				new_world();
			}
			if (ImGui::MenuItem("Open", "Ctrl-O")) {
				m_bGUILevelSelectorOpen = true;
			}
			if (ImGui::MenuItem("Save", "Ctrl-S")) {
				m_bGUILevelSelectorOpen = true;
			}
			if (ImGui::MenuItem("Quit")) {
				m_bShutdown = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ImGui::Begin("Tools")) {
		for (size_t i = (size_t)E_LED_TOOL_MIN; i < (size_t)E_LED_TOOL_MAX; i++) {
			ImGui::ImageButton((ImTextureID)m_icons[(mapeditor::tool)i], ImVec2(64, 64));
		}
	}
	ImGui::End();

	if (m_bGUILevelSelectorOpen) {
		if (ImGui::LevelSelector(m_szGUILevelSelectorMap.data(), m_szGUILevelSelectorMap.size())) {
			// TODO: level loading logic here
			PRINT_DBG("User has selected map \"" << m_szGUILevelSelectorMap.data() << '\"');
		}
	}
}

void mapeditor::draw_world() {
	glm::mat4 proj = glm::perspective(glm::radians(110.f), (float)gpGfx->width() / (float)gpGfx->height(), 0.0f, 1000.0f);
	//glm::mat4 proj = glm::perspective(glm::radians(120.f), 1.0f, 0.0f, 1000.0f);
	//glm::mat4 proj = glm::ortho(0.0f, 512.f, 512.f, 0.0f, 0.0f, 1000.0f);
	vec3 campos = m_camera.get_pos();
	glm::mat4 view = glm::translate(glm::mat4(1.0f), (glm::vec3)campos);
	view = m_camera.get_rot() * view;
	
	for (const auto& object : m_objects) {
		gfx::material pMaterial = gpGfx->model_material(object.iModel);
		int iShader = pMaterial.get_shader();
		if (iShader != -1) {
			gfx::shader_program* pShader = gpGfx->get_shader(iShader);
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), (glm::vec3)object.vecPos) * glm::eulerAngleXYZ(object.vecRot.x(), object.vecRot.y(), object.vecRot.z());
			gpGfx->bind_model(object.iModel);
			pShader->use();
			pShader->use_material(pMaterial);

			pShader->set_mat_view(glm::value_ptr(view));
			pShader->set_mat_proj(glm::value_ptr(proj));
			pShader->set_mat_trans(glm::value_ptr(trans));

			gpGfx->draw_model();
		}
	}
}

void mapeditor::new_world() {
	m_objects.clear();
}

void mapeditor::load_icon(mapeditor::tool id, const std::string & filename) {
	uint32_t iTex = gpGfx->load_texture(filename);
	void* tex = (void*)(((uint64_t)0) | iTex);
	m_icons.emplace(id, tex);
}

void mapeditor::add_object(const char* szFilename) {
	world_object obj;
	obj.szModel = szFilename;
	obj.iModel = gpGfx->load_model(obj.szModel);
	m_objects.push_back(obj);
}
