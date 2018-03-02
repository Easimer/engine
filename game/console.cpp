#include "stdafx.h"
#include "console.h"

#include <list>
#include <string>
#include "gui/imgui.h"

static char console_output_buf[4096] = { 0 };
static char console_input_buf[128] = { 0 };

static std::list<std::string> console_lines;

void console_draw_frame()
{
	if (!gpGlobals->bConsoleOpen)
		return;
	if (ImGui::Begin("Console", &gpGlobals->bConsoleOpen, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped("%s", console_output_buf);
		//ImGui::InputTextMultiline("", console_output_buf, 4096, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
		if (ImGui::InputText("", console_input_buf, 128))
		{
			if (console_execute(console_input_buf))
			{
				console_input_buf[0] = '\0';
			}
		}
	}
	ImGui::End();
}

bool console_execute(const char* szCmd)
{
	console_print_line(std::string("] ") + szCmd);

	return true;
}

void console_print_line(const std::string& line)
{
	console_lines.push_back(line);

	if (console_lines.size() > 32)
	{
		console_lines.push_back(line);
	}

	console_output_buf[0] = '\0';

	size_t nLen = 0;

	for (auto it = console_lines.end(); it != console_lines.begin(); --it)
	{
		nLen += (*it).size();
		if (nLen > 4096) {
			for (auto it2 = it; it2 != console_lines.end(); ++it2)
			{
				strncat(console_output_buf, (*it2).c_str(), 4096);
			}
			break;
		}
	}
}
