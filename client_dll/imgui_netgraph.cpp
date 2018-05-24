#include "stdafx.h"
#include "imgui_netgraph.h"
#include <gui/imgui.h>
#include <gui/imgui_internal.h>
#include <net/networking.h>

using namespace ImGui;
void ImGui::NetGraph(const std::vector<net::packet_stat>& stat) {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	float flColumnWidth = (window->SizeFull.x - 10) / stat.size();
	
	const ImVec2 frame_rb(window->DC.CursorPos.x + window->SizeFull.x - 10, window->DC.CursorPos.y + 200);
	const ImRect frame_bb(window->DC.CursorPos, frame_rb);

	RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

	size_t pkt = 0;
	size_t total_sum = 0;
	for (auto& pkttype : stat) {
		float ratios[net::E_PKSTAT_MAX];
		size_t sum = 0;
		for (size_t i = 0; i < (size_t)net::E_PKSTAT_MAX; i++) {
			sum += pkttype.packet_siz[i];
		}
		total_sum += sum;
		if (sum < 250000)
			sum = 250000;

		for (size_t i = 0; i < (size_t)net::E_PKSTAT_MAX; i++) {
			ratios[i] = (float)pkttype.packet_siz[i] / (float)sum;
		}
		float y = frame_bb.Max.y;
		float h = frame_bb.Max.y - frame_bb.Min.y;
		for (size_t i = 0; i < (size_t)net::E_PKSTAT_MAX; i++) {
			ImVec2 top = frame_bb.Min;
			ImVec2 bot = frame_bb.Max;
			top.x += pkt * flColumnWidth;
			bot.x = top.x + flColumnWidth;
			top.y = y - h * ratios[i];
			bot.y = y;
			y = top.y;
			window->DrawList->AddRectFilled(top, bot, net::stat_type_colors[i]);
		}
		pkt++;
	}

	ItemSize(frame_bb);
	ItemAdd(frame_bb, 0);

	float kbps = total_sum / 1024.f / 128.f;
	ImGui::Text("net: %f kB/s", kbps);
}
