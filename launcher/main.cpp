#include "stdafx.h"
#include "iserver.h"
#include "iclient.h"
#include <type_traits>
#include <chrono>
#include <thread>

#if defined(PLAT_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(PLAT_LINUX)
#include <dlfcn.h>
#endif

#define ASSERT_WINDOWS(expr) \
	{ \
		if(!(expr)) \
		{ \
			wchar_t errormsg[1024]; \
			wchar_t winerr[512]; \
			int errcode = GetLastError(); \
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errcode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), winerr, 512, NULL); \
			swprintf(errormsg, 1024, L"Assertion failed: %s\nWindows error: %s (%d)\nBreak in debugger?\n\nLine: %u\nFile: %s\nFunction:\n%s\n", WIDE1(#expr), winerr, errcode, __LINE__, WFILE, WSIGN); \
			int nRet = MessageBoxW( NULL, errormsg, L"Engine Error",  MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON1); \
			if(nRet == IDYES) \
				DebugBreak(); \
			else if (nRet == IDCANCEL) \
				exit(1); \
		} \
	}

template<typename T>
typename std::enable_if<std::is_pointer<T>::value, T>::type link_dll(const std::string& module, const std::string& symbol) {
#if defined(PLAT_WINDOWS)
	HINSTANCE module_hnd = LoadLibraryA(module.c_str());
	ASSERT_WINDOWS(module_hnd);
	if (module_hnd) {
		T func = (T)GetProcAddress(module_hnd, symbol.c_str());
		ASSERT_WINDOWS(func);
		return func;
	} else {
		return nullptr;
	}
#elif defined(PLAT_LINUX)
	auto module_hnd = dlopen(module.c_str());
	ASSERT_CUSTOM(module_hnd, dlerror());
	if (module_hnd) {
		T func = dlsym(module_hnd, symbol.c_str());
		ASSERT_CUSTOM(func, dlerror());
		return func;
	} else {
		return nullptr;
	}
#endif
}

int main(int argc, char** argv) {
	CMDLINE_INIT();

	char buf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buf);
	std::cout << buf << std::endl;

	std::flush(std::cout);

	auto server_init = link_dll<iserver*(*)()>("bin/server_dll.dll", "server_dll_init");
	auto server_shutdown = link_dll<void(*)(iserver*)>("bin/server_dll.dll", "server_dll_shutdown");

	auto client_init = link_dll<iclient*(*)()>("bin/client_dll.dll", "client_dll_init");
	auto client_shutdown = link_dll<void(*)(iclient*)>("bin/client_dll.dll", "client_dll_shutdown");

	ASSERT(server_init);
	ASSERT(server_shutdown);
	ASSERT(client_init);
	ASSERT(client_shutdown);

	// Start client

	auto cli = client_init();
	ASSERT(cli);
	cli->init("127.0.0.1", "LOCALUSER");

	iserver* srv = nullptr;

	while (true) {
		if (cli && cli->is_shutdown()) break;
		if (srv && srv->is_shutdown()) break;
		std::this_thread::sleep_for(std::chrono::duration<float>(1.f));
		if (cli->request_server() && !srv) {
			srv = server_init();
			ASSERT(srv);
			srv->init();
		}
	}

	if (cli) {
		std::cout << "Stopping client: "; cli->shutdown(); client_shutdown(cli); std::cout << "OK" << std::endl;
	}
	if (srv) {
		std::cout << "Stopping server: "; srv->shutdown(); server_shutdown(srv); std::cout << "OK" << std::endl;
	}

	CMDLINE_SHUTDOWN();
	return 0;
}
