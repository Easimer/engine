#include "stdafx.h"
#include <ifsys/ifsys.h>
#include "mapeditor.h"
#include <gui/imgui.h>
#include <gfx/window_register.h>

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
	m_thread = std::thread([&]() {
		PRINT_DBG("Level editor init!");
		gpGfx->init("engine level editor", 1600, 900);
		// Load GUI icons
		load_icon(E_LED_TOOL_SELECT, "data/textures/gui/mapeditor/tool_select.png");
		load_icon(E_LED_TOOL_TERRAIN_RAISE, "data/textures/gui/mapeditor/tool_terrain_raise.png");
		load_icon(E_LED_TOOL_TERRAIN_LOWER, "data/textures/gui/mapeditor/tool_terrain_lower.png");

		gpGfx->load_default_shaders();

		auto pWndObjects = gfx::gpWindowRegister->create("GUIObjects");

		gpGfx->add_window(pWndObjects);

		PRINT_DBG("EEEEEEEEEEEEEEEEEEEEE");
		PRINT_DBG(pWndObjects->get_title());

		while (!m_bShutdown) {
			handle_events();
			gpGfx->begin_frame();
			draw_gui();
			gpGfx->draw_windows();
			gpGfx->end_frame();
		}
		gpGfx->shutdown();
	});
}

bool mapeditor::shutdown() {
	m_bShutdown = true;
	PRINT_DBG("mapeditor::shutdown");
	if (m_thread.joinable()) {
		m_thread.join();
		return false;
	}
	return true;
}

void mapeditor::handle_events() {
	std::vector<SDL_Event> ev;
	gpGfx->get_events(ev);

	for (auto& e : ev) {
		switch (e.type) {
		case SDL_KEYUP:
			if (e.key.keysym.sym == SDLK_z) {
				m_bFreeCamera = !m_bFreeCamera;
			}
			break;
		case SDL_QUIT:
			m_bShutdown = true;
			break;
		default:
			gpGfx->gui_send_event(e);
			break;
		}
	}
}

void mapeditor::draw_gui() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "Ctrl-N")) {

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

void mapeditor::new_world() {

}

void mapeditor::load_icon(mapeditor::tool id, const std::string & filename) {
	uint32_t iTex = gpGfx->load_texture(filename);
	void* tex = (void*)(((uint64_t)0) | iTex);
	m_icons.emplace(id, tex);
}
