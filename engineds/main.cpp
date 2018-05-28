#include "stdafx.h"
#include "iserver.h"
#include <type_traits>
#include <chrono>
#include <thread>
#include <dl.h>
#include <cpuid.h>
#include <ifsys/ifsys.h>

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

	ifsys is;

	auto srv_fn = LINK_MODULE(pszServerDLL);
	srv_fn(&is);

	auto srv = (iserver*)is.query("GameServer0001");
	ASSERT(srv);
	srv->init();

	while (!srv->is_shutdown()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (srv) {
		std::cout << "Stopping server: "; srv->shutdown(); std::cout << "OK" << std::endl;
	}
	return 0;
}
