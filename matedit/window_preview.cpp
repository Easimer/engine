#include "stdafx.h"
#include <gfx/gfx.h>
#include <gui/imgui.h>
#include "window_preview.h"

void window_preview::draw_content()
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