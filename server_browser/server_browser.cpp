#include "stdafx.h"
#include <thread>
#include <chrono>
#include <net/client.h>
#include <gfx/gfx.h>
#include "server_list.h"
#include "renderer.h"

int main(int argc, char** argv) {
	renderer renderer;
	server_list srvl;

#if defined(PLAT_WINDOWS)
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	net::server_discovery srvd;
	bool bShutdown = false;
	auto thread_sd = std::thread([&]() {
		srvd.timeout(1);
		std::chrono::time_point<std::chrono::high_resolution_clock> next_mc = std::chrono::high_resolution_clock::now();
		while (!bShutdown) {
			if (next_mc > std::chrono::high_resolution_clock::now())
				continue;
			srvd.probe();
			srvd.fetch();
			for (auto& srv : srvd.get()) {
				srvl.add_server(srv);
			}
			next_mc += std::chrono::seconds(1);
		}
	});
	while (!bShutdown) {
		renderer.begin_frame();
		srvl.render();
		if (renderer.handle_events()) {
			bShutdown = true;
		}
		renderer.end_frame();
	}
	thread_sd.join();
	return 0;
}
