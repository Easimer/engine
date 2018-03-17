#include "stdafx.h"
#include <iostream>
#include <thread>
#include <enl/cmdbuf.h>
#include <logger_shared.h>
#include <logger_server.h>
#include <ctime>
#include <vector>
#include "renderer.h"
#include "gui/imgui.h"

#define COLW_TIMESTAMP 128
#define COLW_APP 84
#define COLW_SEVERITY 64
#define COLW_MESSAGE (640 - (COLW_TIMESTAMP + COLW_APP + COLW_SEVERITY))

CMDBUF_BEGIN_CMD(logger_cmdbuf_msg)
logger::message msg;
CMDBUF_END_CMD(logger_cmdbuf_msg)

CMDBUF_DEF(logger_cmdbuf, logger_cmdbuf_msg, 64, true, false);

static logger_cmdbuf lcbuf;
static bool bShutdown = false;

void thread_server() {
	logger_server srv;
	logger::message msg;
	std::vector<logger::message> messages;
	while (!bShutdown) {
		if (srv.serve_one(msg)) {
			messages.push_back(msg);
			if (lcbuf.is_empty()) {
				lcbuf.begin_write();
				for (auto& m : messages) {
					logger_cmdbuf_msg cmdbuf_msg = { m };
					lcbuf.write(cmdbuf_msg);
				}
				lcbuf.end_write();
				messages.clear();
			}			
		}
	}
}

struct message {
	message(const logger::message& msg) {
		strftime(timestamp, 64, "%Y-%m-%d %H:%M:%S", localtime((const time_t*)&msg.timestamp));
		strncpy(application, msg.application, 16);
		severity = logger::strseverity((logger::severity)msg.severity);
		strncpy(content, msg.content, 512);
		
		this->msg = msg;
	}

	char timestamp[64];
	char application[16];
	const char* severity;
	char content[512];

	logger::message msg;
};



int main(int argc, char** argv) {

	renderer renderer;

	std::thread server_thread(thread_server);

	std::vector<message> messages;

	while (1) {

		logger_cmdbuf_msg* pMessages;
		size_t nMessages;

		renderer.begin_frame();
		if (ImGui::Begin("engine logger", 0, ImVec2(1100, 650), -1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoTitleBar)) {
			ImGui::SameLine();
			if (ImGui::Button("Quit")) {
				break;
			}
			ImGui::NewLine();
			ImGui::Columns(4);
			ImGui::Text("Time"); ImGui::NextColumn();
			ImGui::Text("Application"); ImGui::NextColumn();
			ImGui::Text("Severity"); ImGui::NextColumn();
			ImGui::Text("Message"); ImGui::NextColumn();
			ImGui::Separator();
			ImGui::Columns(1);
			if(ImGui::BeginChild("##messages")) {
				ImGui::Columns(4);
				for (auto& msg : messages) {
					ImVec4 col(1.0, 1.0, 1.0, 1.0);
					switch (msg.msg.severity) {
					case logger::severity::DEBUG:
						col = ImVec4(1.0, 1.0, 0.0, 1.0);
						break;
					case logger::severity::WARNING:
						col = ImVec4(0.8, 0.6, 0.0, 1.0);
						break;
					case logger::severity::INFO:
						break;
					case logger::severity::ERROR:
						col = ImVec4(1.0, 0.0, 0.0, 1.0);
						break;
					case logger::severity::EMERGENCY:
						col = ImVec4(0.6, 0.0, 0.0, 1.0);
						break;
					}
					ImGui::TextColored(col, msg.timestamp); ImGui::NextColumn();
					ImGui::TextColored(col, msg.application); ImGui::NextColumn();
					ImGui::TextColored(col, msg.severity); ImGui::NextColumn();
					ImGui::TextColored(col, msg.content); ImGui::NextColumn();
				}
				ImGui::EndChild();
			}
			ImGui::End();
		} else {
			break;
		}
		renderer.end_frame();

		if (renderer.handle_events())
		{
			break;
		}

		if (lcbuf.begin_read(&pMessages, &nMessages)) {
			if (nMessages) {
				while (nMessages--) {
					messages.push_back(message(pMessages->msg));
					pMessages++;
				}
			}
			lcbuf.end_read();
		}
	}

	bShutdown = true;
	server_thread.join();

	return 0;
}
