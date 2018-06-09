#include "stdafx.h"
#include "iserver.h"
#include <type_traits>
#include <chrono>
#include <thread>
#include <dl.h>
#include <cpuid.h>
#include <ifsys/ifsys.h>
#include <elf/imapeditor.h>

int main(int argc, char** argv) {
#if defined(PLAT_WINDOWS)
	char buf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buf);
	std::cout << buf << std::endl;
#endif
	std::flush(std::cout);

	ifsys is;

	auto sModuleFilename = module_filename("elf");
	const char* pszModuleFilename = sModuleFilename.c_str();
	std::cout << "Module: " << sModuleFilename << std::endl;

	auto elf_fn = LINK_MODULE(pszModuleFilename);
	elf_fn(&is);

	auto elf = (imapeditor*)is.query("EngineLevelEditor0001");
	ASSERT(elf);
	elf->init();

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (elf) {
		std::cout << "Stopping editor: "; elf->shutdown(); std::cout << "OK" << std::endl;
	}
	return 0;
}
