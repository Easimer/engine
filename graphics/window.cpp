#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/window.h>
#include "gui/imgui.h"

using namespace gfx;

void window::draw_window()
{
	if(ImGui::Begin(get_title(), nullptr, ImVec2(min_width(), min_height()))) {
		draw_content();
		ImGui::End();
	}
}
