#include "stdafx.h"
#include "imgui_netgraph.h"
#include <gui/imgui.h>
#include <gui/imgui_internal.h>
#include <net/networking.h>

using namespace ImGui;

static void DrawColorGuide(ImU32 color, const char* pszName) {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	ImVec2 p(window->DC.CursorPos.x + g.FontSize, window->DC.CursorPos.y + g.FontSize);
	ImRect cgbb(window->DC.CursorPos, p);
	window->DrawList->AddRectFilled(window->DC.CursorPos, p, color);

	ItemSize(cgbb);
	ItemAdd(cgbb, 0);

	ImGui::SameLine();
	ImGui::Text(pszName);
}

void ImGui::NetGraph(const std::vector<net::packet_stat>& stat) {
	ImGui::SetNextWindowSize(ImVec2(600, 400));
	ImGui::Begin("netgraph");
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	float flColumnWidth = (window->SizeFull.x - 10) / stat.size();

	const ImVec2 frame_rb(window->DC.CursorPos.x + window->SizeFull.x - 10, window->DC.CursorPos.y + (window->SizeFull.y / 2));
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
		if (sum < 2048)
			sum = 2048;

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

	// Draw color guide
	DrawColorGuide(net::stat_type_colors[net::E_PKSTAT_CLIENT_UPDATE], "Client update");
	DrawColorGuide(net::stat_type_colors[net::E_PKSTAT_ENTITY_UPDATE], "Entity update");
	DrawColorGuide(net::stat_type_colors[net::E_PKSTAT_OTHER], "Other");

	ImGui::Separator();

	float kbps = total_sum / 1024.f / 128.f;
	ImGui::Text("net: %f kB/s", kbps);

	ImGui::End();
}
