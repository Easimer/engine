#include "stdafx.h"
#include <gfx/gfx.h>
#include "window_model.h"
#include "gui/imgui.h"

//window_model::window_model(const model & mdl, const gfx::material & mat)
//{
//}

void window_model::draw_content()
{
	auto pos = ImGui::GetWindowPos();
	auto min = ImGui::GetWindowContentRegionMin();
	auto max = ImGui::GetWindowContentRegionMax();

	int sx = min.x + pos.x;
	int sy = min.y + pos.y;
	int ex = max.x + pos.x;
	int ey = max.y + pos.y;

	gpGfx->set_viewport(sx, sy, ex, ey);
}
