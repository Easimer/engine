#include "stdafx.h"
#include "mainmenu.h"
#include <gui/imgui.h>
#include <gfx/gfx.h>

mainmenu::mainmenu() {
}

mainmenu::exitcode mainmenu::tick() {
	mainmenu::exitcode ret = EMMENU_OK;
	ImGui::Begin("");

	if (ImGui::Button("Join Game")) {
		ret = EMMENU_JOIN_REMOTE_GAME;
	}
	if (ImGui::Button("Host Game")) {
		ret = EMMENU_START_LOCAL_GAME;
	}
	if (ImGui::Button("Quit")) {
		ret = EMMENU_QUIT_GAME;
	}

	ImGui::End();
	gpGfx->handle_events();

	return ret;
}
