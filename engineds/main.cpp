#include "stdafx.h"
#include "iserver.h"
#include <type_traits>
#include <chrono>
#include <thread>
#include <dl.h>

#if defined(PLAT_WINDOWS)
const char* pszServerDLL = "bin/server_dll.dll";
#elif defined(PLAT_LINUX)
const char* pszServerDLL = "bin/libserver_dll.so";
#endif

int main(int argc, char** argv) {
#if defined(PLAT_WINDOWS)
	char buf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buf);
	std::cout << buf << std::endl;
#endif
	std::flush(std::cout);

	auto server_init = link_dll<iserver*>(pszServerDLL, "server_dll_init");
	auto server_shutdown = link_dll<void, iserver*>(pszServerDLL, "server_dll_shutdown");

	ASSERT(server_init);
	ASSERT(server_shutdown);

	// Start client
	iserver* srv = server_init();
	ASSERT(srv);
	srv->init();

	while (!srv->is_shutdown()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (srv) {
		std::cout << "Stopping server: "; srv->shutdown(); server_shutdown(srv); std::cout << "OK" << std::endl;
	}
	return 0;
}
