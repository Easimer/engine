#include "stdafx.h"
#include "iserver.h"
#include "iclient.h"
#include <type_traits>
#include <chrono>
#include <thread>
#include <dl.h>

#if defined(PLAT_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(PLAT_LINUX)
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

#if defined(PLAT_WINDOWS)
const char* pszServerDLL = "bin/server_dll.dll";
const char* pszClientDLL = "bin/client_dll.dll";
#elif defined(PLAT_LINUX)
const char* pszServerDLL = "bin/libserver_dll.so";
const char* pszClientDLL = "bin/libclient_dll.so";
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

/// Set the current working directory to the game root directory
/// (we're launching from /bin/)
bool set_workdir() {
#if defined(PLAT_WINDOWS)
	DWORD ftyp = GetFileAttributesA("data");
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY && ftyp != INVALID_FILE_ATTRIBUTES) {
		PRINT_DBG("Already running from game rootdir!");
		return true;
	}

	SetCurrentDirectory("..");
	ftyp = GetFileAttributesA("data");
	if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY) || ftyp == INVALID_FILE_ATTRIBUTES) {
		PRINT_ERR("Switched to parent dir, but data/ doesn't exist!");
		return false;
	}
	return true;
#elif defined(PLAT_LINUX)
	if (chdir("..") == 0) {
		DIR* pDir = opendir("data");
		if (pDir) {
			closedir(pDir);
			return true;
		} else {
			PRINT_ERR("Switched to parent dir, but data/ doesn't exist!");
		}
	} else {
		PRINT_ERR("Couldn't switch to parent dir!");
	}
	return false;
#endif
}

int main(int argc, char** argv) {
	if (!set_workdir()) {
		PRINT_ERR("Couldn't switch to rootdir!!!");
	}
#if defined(PLAT_WINDOWS)
	char buf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buf);
	std::cout << buf << std::endl;
#elif defined(PLAT_LINUX)
	char buf[1024];
	getcwd(buf, 1024);
	std::cout << buf << std::endl;
#endif
	std::flush(std::cout);

	auto server_init = link_dll<iserver*>(pszServerDLL, "server_dll_init");
	auto server_shutdown = link_dll<void, iserver*>(pszServerDLL, "server_dll_shutdown");

	auto client_init = link_dll<iclient*>(pszClientDLL, "client_dll_init");
	auto client_shutdown = link_dll<void, iclient*>(pszClientDLL, "client_dll_shutdown");

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
		if (!srv && cli->request_server()) {
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

	return 0;
}
