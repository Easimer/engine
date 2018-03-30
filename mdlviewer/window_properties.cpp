#include "stdafx.h"
#include "window_properties.h"
#include "gui/imgui.h"
#include <gfx/gfx.h>
#include <gfx/material.h>
#if defined(PLAT_WINDOWS)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commdlg.h>
#endif

extern gfx::model_id giCurrentModel;

void window_properties::draw_content()
{
#if defined(PLAT_WINDOWS)
	if (ImGui::Button("Open file")) {
		char filename[MAX_PATH];
		OPENFILENAMEA ofn;

		memset(filename, 0, MAX_PATH * sizeof(char));
		memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = "Engine Model Files (*.emf)\0*.emf\0studiomdl data (*.smd)\0*.smd\0Any file\0*.*\0";
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = "Open a model file";
		ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn)) {
			PRINT_DBG("Attempting to open " << filename);
			if(giCurrentModel)
				gpGfx->unload_model(giCurrentModel);
			giCurrentModel = gpGfx->load_model(std::string(filename));
			PRINT_DBG("New model ID: " << giCurrentModel);
		}
	}
#else
	ImGui::Text("To open a file, specify it on the command line!");
#endif
	if (!giCurrentModel) return;

	bool bHasCollider = gpGfx->model_has_collider(giCurrentModel);
	bool bTrue = true;
	const gfx::material& material = gpGfx->model_material(giCurrentModel);
	const std::string& material_fn = gpGfx->model_material_filename(giCurrentModel);

	ImGui::BeginGroup();
	ImGui::Text("Properties");
	ImGui::Separator();
	ImGui::Text("Number of vertices: %u", gpGfx->model_vertices(giCurrentModel));
	ImGui::Separator();
	ImGui::InputText("Material", (char*)material_fn.c_str(), material_fn.size(), ImGuiInputTextFlags_ReadOnly);
	ImGui::InputText("Shader", (char*)material.get_shader_name().c_str(), material.get_shader_name().size(), ImGuiInputTextFlags_ReadOnly);
	ImGui::Separator();
	ImGui::SameLine();
	ImGui::Text("Qualified to be: ");
	ImGui::Checkbox("prop_dynamic", &bTrue);
	ImGui::Checkbox("prop_physics", &bHasCollider);
	ImGui::Checkbox("prop_static", &bTrue);
	ImGui::NewLine();
	ImGui::EndGroup();
}
