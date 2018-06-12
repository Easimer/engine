#include "stdafx.h"
#include "gui_objects.h"
#include <gfx/gfx.h>
#include <gfx/shader.h>
#include <gfx/shader_program.h>
#include <gfx/window.h>
#include <gfx/window_register.h>
#include <gfx/framebuffer.h>

#include <gui/imgui.h>
#include <gui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

namespace ImGui {

bool ObjectButton(ImTextureID tex, const char* label, ImVec2& size) {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	ImGui::BeginGroup();

	// Default to using texture ID as ID. User can still push string/integer prefixes.
	// We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
	PushID((void *)tex);
	const ImGuiID id = window->GetID("#image");
	PopID();

	const ImVec2 padding = style.FramePadding;
	ImVec2 br(window->DC.CursorPos.x + size.x + padding.x * 2, window->DC.CursorPos.y + size.y + padding.y * 2 + ImGui::GetTextLineHeight());
	const ImRect bb(window->DC.CursorPos, br);
	ImVec2 image_tl(window->DC.CursorPos.x + padding.x, window->DC.CursorPos.y + padding.y);
	ImVec2 image_br(window->DC.CursorPos.x + size.x + padding.x, window->DC.CursorPos.y + size.y + padding.y);
	const ImRect image_bb(image_tl, image_br);
	ItemSize(bb);
	if (!ItemAdd(bb, id)) {
		ImGui::EndGroup();
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	// Render
	const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
	window->DrawList->AddImage(tex, image_bb.Min, image_bb.Max, ImVec2(0, 0), ImVec2(1, 1), GetColorU32(ImVec4(1, 1, 1, 1)));

	ImGui::Text(label);

	ImGui::EndGroup();

	return pressed;
}

}

void gui_objects::set_is(ifsys * is) {
	m_pISys = is;
	m_pMapEditor = (imapeditor*)is->query("EngineLevelEditor0001");
}

void gui_objects::draw_content() {
	if (ImGui::Button("Refresh")) {
		m_stack_path.clear();
		m_cur_entries = list_files();
	}
	if (m_cur_entries.size() == 0) {
		// Init
		m_cur_entries = list_files();
	}
	ImVec2 button_size(64, 64);
	int nMaxPerRow = ImGui::GetWindowWidth() / button_size.x;
	size_t iRowIndex = 0;
	std::string sCwd = cwd();
	for (const auto& e : m_cur_entries) {
		if (e.filename.size() == 0)
			continue;
		if (e.bDirectory) {
			if (e.bUp) {
				if (ImGui::Button("..", button_size)) {
					m_stack_path.pop_back();
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
			// Get model ID from filename
			if (m_object_cache.count(filename)) {
				id = m_object_cache[filename];
			} else {
				id = gpGfx->load_model(filename);
				m_object_cache[filename] = id;
			}
			// Retrieve preview texture
			uint64_t iTex;
			if (m_preview_cache.count(id)) {
				iTex = m_preview_cache[id]->handle();
			} else {
				iTex = generate_preview(id)->handle();
			}
			if (ImGui::ObjectButton((ImTextureID)iTex, e.filename.c_str(), button_size)) {
				PRINT_DBG("Selected object " << filename);
				if (m_pMapEditor) {
					m_pMapEditor->add_object(filename.c_str());
				}
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
		e_up.filename = "Up";
		ret.push_back(e_up);
	}
	// Win32 implementation
#if defined(PLAT_WINDOWS)
	WIN32_FIND_DATA ffd;
	HANDLE hFile;
	std::string path = "data/models/";
	for (const auto& dir : m_stack_path) {
		path.append(dir);
		path.append("/");
	}
	path.append("*");
	hFile = FindFirstFileA(path.c_str(), &ffd);
	while (hFile != INVALID_HANDLE_VALUE) {
		if (ffd.cFileName[0] != '.') {
			gui_objects::dir_entry e;
			e.bDirectory = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			e.bUp = false;
			e.filename = std::string(ffd.cFileName);
			ret.push_back(e);
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
	return ret;
}

gfx::shared_tex2d gui_objects::generate_preview(gfx::model_id id) {
	PRINT_DBG("gui_objects: generating preview for model " << id);
	// Render model
	// Get the model's material
	gfx::material pMaterial = gpGfx->model_material(id);
	// Get the material's shader index
	int iShader = pMaterial.get_shader();
	if (iShader != -1) {
		gfx::shader_program* pShader = gpGfx->get_shader(iShader);
		gpGfx->clear();
		gpGfx->bind_model(id);
		pShader->use();
		pShader->use_material(pMaterial);

		glm::mat4 proj = glm::perspective(glm::radians(110.f), (float)gpGfx->width() / (float)gpGfx->height(), 0.0f, 1000.0f);
		//glm::mat4 proj = glm::perspective(glm::radians(120.f), 1.0f, 0.0f, 1000.0f);
		//glm::mat4 proj = glm::ortho(0.0f, 512.f, 512.f, 0.0f, 0.0f, 1000.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5f, -0.75f));
		view = glm::rotate(view, glm::radians(180.f), glm::vec3(0, 0, 1));
		glm::mat4 trans(1.0f);

		pShader->set_mat_view(glm::value_ptr(view));
		pShader->set_mat_proj(glm::value_ptr(proj));
		pShader->set_mat_trans(glm::value_ptr(trans));
		gfx::shared_fb fb = std::make_shared<gfx::framebuffer>();
		fb->bind();
		gpGfx->clear();
		gpGfx->draw_model();
		fb->unbind();
		
		auto texDiffuse = fb->diffuse();

		m_preview_cache[id] = texDiffuse;
		PRINT_DBG("Generated!");
		return texDiffuse;
	}
	return gfx::shared_tex2d();
}

REGISTER_WINDOW(gui_objects, "GUIObjects");
