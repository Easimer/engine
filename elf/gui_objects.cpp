#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/shader.h>
#include <gfx/shader_program.h>
#include <gfx/window.h>
#include <gfx/window_register.h>

#include <gui/imgui.h>
#include <gui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

class gui_objects : public gfx::window {
public:
	virtual const char* get_title() override { return "Objects"; }
	virtual float min_width() override { return gpGfx->width(); }
	virtual float min_height() override { return 300; }
protected:
	virtual void draw_content() override;

	struct dir_entry {
		bool bDirectory;
		bool bUp;
		std::string filename;
	};

	// Returns a list of files and directories
	std::vector<dir_entry> list_files() const;

	const std::string cwd() const {
		std::string path = "data/models/";
		for (const auto& dir : m_stack_path) {
			path.append(dir);
			path.append("/");
		}
		return path;
	}
private:
	std::vector<std::string> m_stack_path;
	std::vector<dir_entry> m_cur_entries;

	std::map<std::string, gfx::model_id> m_object_cache;

	std::vector<uint32_t> m_texture_pool;
};

namespace ImGui {
bool ObjectButton(gfx::model_id id, const ImVec2& size) {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImVec2 p(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y);
	ImRect bb(window->DC.CursorPos, p);

	int VPX1 = window->DC.CursorPos.x;
	int VPY1 = window->DC.CursorPos.y;
	int VPX2 = window->DC.CursorPos.x + size.x;
	int VPY2 = window->DC.CursorPos.y + size.y;

	ItemSize(bb);
	ItemAdd(bb, 0);

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, 0);

	// Render button frame
	const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

	// Render model
	
	// Get the model's material
	gfx::material pMaterial = gpGfx->model_material(id);
	// Get the material's shader index
	int iShader = pMaterial.get_shader();
	if (iShader != -1) {
		gfx::shader_program* pShader = gpGfx->get_shader(iShader);
		ImVec2 vTL = bb.GetTL();
		ImVec2 vBR = bb.GetBR();
		gpGfx->set_viewport(vTL.x, vTL.y, vBR.x, vBR.y);
		gpGfx->clear();
		gpGfx->bind_model(id);
		pShader->use();
		pShader->use_material(pMaterial);

		glm::mat4 proj = glm::perspective(glm::radians(90.f), (float)gpGfx->width() / (float)gpGfx->height(), 0.0f, 1000.0f);
		//glm::mat4 proj = glm::ortho(0.0f, (float)gpGfx->width(), (float)gpGfx->height(), 0.0f, 0.0f, 1000.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -1));
		glm::mat4 trans(1.0f);

		pShader->set_mat_view(glm::value_ptr(view));
		pShader->set_mat_proj(glm::value_ptr(proj));
		pShader->set_mat_trans(glm::value_ptr(trans));
		
		gpGfx->draw_model();
		gpGfx->restore_viewport();
	}

	return pressed;
}
}

void gui_objects::draw_content() {
	ImGui::Button("Refresh");
	if (m_cur_entries.size() == 0) {
		// Init
		m_cur_entries = list_files();
	}
	ImVec2 button_size(64, 64);
	int nMaxPerRow = ImGui::GetWindowWidth() / button_size.x;
	size_t iRowIndex = 0;
	std::string sCwd = cwd();
	for (const auto& e : m_cur_entries) {
		if (e.bDirectory) {
			if (e.bUp) {
				if (ImGui::Button("..", button_size)) {
					m_cur_entries.pop_back();
					m_cur_entries = list_files();
				}
			} else {
				if (ImGui::Button(e.filename.c_str(), button_size)) {
					m_stack_path.push_back(e.filename);
					PRINT_DBG("Entering directory " << e.filename);
					m_cur_entries = list_files();
				}
			}
		} else {
			gfx::model_id id;
			std::string filename = sCwd + e.filename;
			if (m_object_cache.count(filename)) {
				id = m_object_cache[filename];
			} else {
				id = gpGfx->load_model(filename);
				m_object_cache[filename] = id;
			}
			if (ImGui::ObjectButton(id, button_size)) {
				PRINT_DBG("Selected object " << filename);
			}
		}
		if (iRowIndex++ < nMaxPerRow) {
			ImGui::SameLine();
		} else {
			iRowIndex = 0;
			ImGui::NewLine();
		}
	}
}

std::vector<gui_objects::dir_entry> gui_objects::list_files() const {
	std::vector<gui_objects::dir_entry> ret;
	// Add implicit entry "Up" if the current dir is not "/"
	if (m_stack_path.size() != 0) {
		gui_objects::dir_entry e_up;
		e_up.bDirectory = e_up.bUp = true;
		ret.push_back(e_up);
	}
	// Win32 implementation
#if defined(PLAT_WINDOWS)
	WIN32_FIND_DATA ffd;
	HANDLE hFile;
	std::string path = "data\\models\\";
	for (const auto& dir : m_stack_path) {
		path.append(dir);
		path.append("\\");
	}
	path.append("*");
	hFile = FindFirstFileA(path.c_str(), &ffd);
	while (hFile != INVALID_HANDLE_VALUE) {
		gui_objects::dir_entry e;
		e.bDirectory = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		e.bUp = false;
		e.filename = std::string(ffd.cFileName);
		ret.push_back(e);
		if (FindNextFileA(hFile, &ffd) == 0)
			break;
	}
	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) {
		PRINT_DBG(dwError);
	}
	FindClose(hFile);
#endif
	return ret;
}

REGISTER_WINDOW(gui_objects, "GUIObjects");
