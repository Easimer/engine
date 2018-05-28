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
