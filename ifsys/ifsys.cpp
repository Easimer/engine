#include "stdafx.h"
#include <ifsys/ifsys.h>
#include <print.h>

void ifsys::connect(const char * pszName, void * pModule) {
	std::string key(pszName);
	if (!m_modules.count(key)) {
		PRINT_DBG("ifsys: " << pszName << " connected");
		m_modules.emplace(key, pModule);
	}
}

void* ifsys::query(const char * pszName) {
	std::string key(pszName);
	if (m_modules.count(key))
		return m_modules[key];
	return nullptr;
}

void ifsys::shutdown() {
	for (auto& m : m_modules) {
		if (m.second) {
			while (!((imodule*)m.second)->shutdown());
			PRINT_DBG("ifsys::shutdown: " << m.first);
		}
	}
}

std::shared_ptr<std::thread> ifsys::make_thread() {
	auto t = std::make_shared<std::thread>();
	m_threads.push_back(t);
	return t;
}

std::string module_filename(const std::string& name) {
#if defined(PLAT_WINDOWS)
	return "bin/" + name + ".dll";
#elif defined(PLAT_LINUX)
	return "bin/lib" + name + ".so";
#else
#error Unsupported platform
#endif
}
