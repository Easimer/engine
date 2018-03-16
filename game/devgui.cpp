#include "stdafx.h"
#include "renderer.h"
#include "gui/imgui_impl_sdl_gl3.h"
#include "icamera.h"
#include "statistics.h"
#include "entsys.h"
#include "devgui.h"
#include "prop_common.h"
#include <glm/glm.hpp>
#include "console.h"

void renderer::draw_debug_tools()
{
	console_draw_frame();
#ifdef PLAT_DEBUG
	if (!gpGlobals->bDevGUI)
		return;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			bool bQuit = false;
			bool bConsole = false;

			ImGui::MenuItem("Console", NULL, &bConsole);
			ImGui::Separator();
			ImGui::MenuItem("Quit", NULL, &bQuit);
			ImGui::EndMenu();

			gpGlobals->bRunning = !bQuit;

			if (bConsole)
				gpGlobals->bConsoleOpen = !gpGlobals->bConsoleOpen;
		}
		if (ImGui::BeginMenu("Entities"))
		{
			bool bOpenCreate = false;
			ImGui::MenuItem("Create", NULL, &bOpenCreate);
			ImGui::EndMenu();

			if (bOpenCreate) {
				gpGlobals->pDevGUI->m_szClassname[0] = '\0';
				gpGlobals->pDevGUI->m_szTargetname[0] = '\0';
				gpGlobals->pDevGUI->m_bShowEntityCreate = true;
			}
		}
		if (ImGui::BeginMenu("Window"))
		{
			bool bToggleRendererDebug = false;
			bool bToggleStatistics = false;
			ImGui::MenuItem("Renderer Debug", NULL, &bToggleRendererDebug);
			ImGui::MenuItem("Engine Statistics", NULL, &bToggleStatistics);
			ImGui::EndMenu();

			if (bToggleRendererDebug)
				gpGlobals->pDevGUI->m_bShowRendererDebug = !gpGlobals->pDevGUI->m_bShowRendererDebug;
			if (bToggleStatistics)
				gpGlobals->pDevGUI->m_bShowStatistics = !gpGlobals->pDevGUI->m_bShowStatistics;
		}
		ImGui::EndMainMenuBar();
	}

	// Renderer Statistics Window

	if (ImGui::Begin("Renderer Debug", &gpGlobals->pDevGUI->m_bShowRendererDebug))
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

		ImGui::Checkbox("Draw normals only", &m_bDrawNormalsOnly);
		ImGui::Checkbox("Draw global light gizmo", &gpGlobals->pDevGUI->m_bDrawGlobalLightGizmo);

		ImGui::End();
	}

	//
	// Engine Statistics Window
	//
	if (ImGui::Begin("Engine Statistics", &gpGlobals->pDevGUI->m_bShowStatistics, ImGuiWindowFlags_AlwaysAutoResize))
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

	if (ImGui::Begin("Entity Inspector", &gpGlobals->pDevGUI->m_bShowInspector))
	{
		// Entity list
		ImGui::Text("Entities");
		std::vector<std::pair<size_t, char[64]>> entities;
		gpGlobals->pEntSys->get_entities(entities);
		
		ImGui::ListBoxHeader("");
		for (size_t i = 0; i < entities.size(); i++)
		{
			char szLabelBuf[256];
			snprintf(szLabelBuf, 256, "%s##%llu\n", entities[i].second, entities[i].first);
			if (ImGui::Selectable(szLabelBuf, gpGlobals->pDevGUI->m_iCurEnt == entities[i].first))
			{
				gpGlobals->pDevGUI->m_iCurEnt = entities[i].first;
			}
		}
		ImGui::ListBoxFooter();

		ImGui::Separator();

		if (gpGlobals->pDevGUI->m_iCurEnt)
		{
			float aflPos[3];
			std::vector<entsys_update_t> entsys_updates;

			auto pEnt = gpGlobals->pEntSys->get_entity(gpGlobals->pDevGUI->m_iCurEnt);
			auto pEntProp = dynamic_cast<c_base_prop*>(pEnt);
			if (pEnt)
			{
				entsys_update_t upd;
				upd.nEntityID = gpGlobals->pDevGUI->m_iCurEnt;

				ImGui::Text(pEnt->get_classname());

				auto vecPos = pEnt->get_abspos();
				aflPos[0] = vecPos[0];
				aflPos[1] = vecPos[1];
				aflPos[2] = vecPos[2];
				if (ImGui::InputFloat3("Position", aflPos))
				{
					upd.iType = ENTSYS_T_SETPOS;
					upd.vector = vec3(aflPos[0], aflPos[1], aflPos[2]);
					entsys_updates.push_back(upd);
				}

				auto vecRot = pEnt->get_relrot();
				aflPos[0] = glm::degrees(vecRot[0]);
				aflPos[1] = glm::degrees(vecRot[1]);
				aflPos[2] = glm::degrees(vecRot[2]);
				if (ImGui::InputFloat3("Rotation", aflPos))
				{
					upd.iType = ENTSYS_T_SETROT;
					upd.vector = vec3(glm::radians(aflPos[0]), glm::radians(aflPos[1]), glm::radians(aflPos[2]));
					entsys_updates.push_back(upd);
				}

				float flScale = pEnt->get_scale();
				if (ImGui::InputFloat("Scale", &flScale))
				{
					upd.iType = ENTSYS_T_SETSCALE;
					upd.flFloat = flScale;
					entsys_updates.push_back(upd);
				}

				if (pEntProp) {
					ImGui::InputText("Model", gpGlobals->pDevGUI->m_szModelPath, 128);
					if (ImGui::Button("Update model")) {
						upd.iType = ENTSYS_T_SETMODEL;
						upd.iszString = std::string(gpGlobals->pDevGUI->m_szModelPath);
						entsys_updates.push_back(upd);
					}
				}

				auto keyvalues = pEnt->get_keyvalues();
				for (const auto& kv : keyvalues) {
					switch (kv.first.second) {
					case KV_T_FLOAT:
						ImGui::InputFloat(kv.first.first.c_str(), pEnt->get_keyvalue<KV_T_FLOAT>(kv.first.first).ptr());
						break;
					case KV_T_RGBA:
						float rgba[4];
						auto col = pEnt->get_keyvalue<KV_T_RGBA>(kv.first.first);

						rgba[0] = col->r;
						rgba[1] = col->g;
						rgba[2] = col->b;
						rgba[3] = col->a;

						ImGui::ColorEdit4(kv.first.first.c_str(), rgba);

						col->r = rgba[0];
						col->g = rgba[1];
						col->b = rgba[2];
						col->a = rgba[3];
						break;
					}
						

				}

				if (ImGui::Button("Kill Entity")) {
					upd.iType = ENTSYS_T_KILL;
					entsys_updates.push_back(upd);
					gpGlobals->pDevGUI->m_iCurEnt = 0;
				}
			}
			else
			{
				PRINT_DBG("devgui: entity handle #" << gpGlobals->pDevGUI->m_iCurEnt << " is invalid!");
			}

			if (entsys_updates.size() > 0)
			{
				gpGlobals->pEntSys->send_updates(entsys_updates);
			}
		}

		ImGui::End();
	}

	if (ImGui::Begin("Create entity", &gpGlobals->pDevGUI->m_bShowEntityCreate))
	{
		ImGui::InputText("Classname: ", gpGlobals->pDevGUI->m_szClassname, 128);
		ImGui::InputText("Targetname: ", gpGlobals->pDevGUI->m_szTargetname, 128);
		if (ImGui::Button("Create")) {
			entsys_update_t upd;
			upd.nEntityID = ENTSYS_T_CREATE;
			upd.iszString = std::string(gpGlobals->pDevGUI->m_szClassname);
			strncpy(upd.szString, gpGlobals->pDevGUI->m_szTargetname, 128);
			gpGlobals->pDevGUI->m_bShowEntityCreate = false;
		}
		ImGui::End();
	}

#endif /* PLAT_DEBUG */
}
