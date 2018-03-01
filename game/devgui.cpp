#include "stdafx.h"
#include "renderer.h"
#include "gui/imgui_impl_sdl_gl3.h"
#include "icamera.h"
#include "statistics.h"
#include "entsys.h"

void renderer::draw_debug_tools()
{
#ifdef PLAT_DEBUG
	if (!gpGlobals->bDevGUI)
		return;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			bool bQuit = false;

			ImGui::Separator();
			ImGui::MenuItem("Quit", NULL, &bQuit);
			ImGui::EndMenu();

			gpGlobals->bRunning = !bQuit;
		}
		if (ImGui::BeginMenu("Window"))
		{
			bool bToggleRendererDebug = false;
			bool bToggleStatistics = false;
			ImGui::MenuItem("Renderer Debug", NULL, &bToggleRendererDebug);
			ImGui::MenuItem("Engine Statistics", NULL, &bToggleStatistics);
			ImGui::EndMenu();

			if (bToggleRendererDebug)
				m_bShowRendererDebug = !m_bShowRendererDebug;
			if (bToggleStatistics)
				m_bShowStatistics = !m_bShowStatistics;
		}
		ImGui::EndMainMenuBar();
	}

	// Renderer Statistics Window

	if (ImGui::Begin("Renderer Debug", &m_bShowRendererDebug))
	{
		ImGui::BeginGroup();
		ImGui::Separator();
		ImGui::Text("FPS: %f\n", m_flFPS);
		ImGui::Text("Engine time: %f\n", gpGlobals->curtime);
		ImGui::Text("Delta: %f\n", gpGlobals->flDeltaTime);
		ImGui::EndGroup();
			
		ImGui::BeginGroup();
		ImGui::Separator();
		ImGui::Text("Camera");
		vec3 cam_pos = gpGlobals->pCamera->get_pos();
		vec3 cam_rot = gpGlobals->pCamera->get_rot();
		ImGui::Text("Position:\t (%f; %f; %f)\n", cam_pos[0], cam_pos[1], cam_pos[2]);
		ImGui::Text("Rotation:\t (%f; %f; %f)\n", cam_rot[0], cam_rot[1], cam_rot[2]);
		ImGui::EndGroup();

		ImGui::End();
	}

	//
	// Engine Statistics Window
	//
	if (ImGui::Begin("Engine Statistics", &m_bShowStatistics, ImGuiWindowFlags_AlwaysAutoResize))
	{
		for (size_t i = 0; i < ESTAT_C_MAX; i++)
		{
			ImGui::BeginGroup();
			ImGui::Separator();
			ImGui::Text(gpGlobals->pStatistics->get_category_name((estat_category)i));
			auto category = gpGlobals->pStatistics->get_category((estat_category)i);
			for (auto& stat : category)
			{
				switch (stat.second.first)
				{
				case ESTAT_T_INT:
				case ESTAT_T_UINT:
					ImGui::InputInt(stat.first.c_str(), &stat.second.second.nVal);
					break;
				case ESTAT_T_FLOAT:
				case ESTAT_T_DOUBLE:
					ImGui::InputFloat(stat.first.c_str(), &stat.second.second.flVal);
					break;
				}
			}
			ImGui::EndGroup();
		}
		ImGui::End();
	}

	//
	// Entity Inspector
	//

	if (ImGui::Begin("Entity Inspector", &m_bShowInspector))
	{
		// Entity list
		ImGui::Text("Entities");
		std::vector<std::pair<size_t, char[64]>> entities;
		gpGlobals->pEntSys->get_entities(entities);
		
		ImGui::ListBoxHeader("Entities");
		for (size_t i = 0; i < entities.size(); i++)
		{
			if (ImGui::Selectable(entities[i].second, i == entities[i].first))
			{
				m_iCurEnt = entities[i].first;
			}
		}
		ImGui::ListBoxFooter();

		ImGui::Separator();
		ImGui::End();
	}

#endif /* PLAT_DEBUG */
}