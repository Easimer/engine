#pragma once

#include <enl/platform.h>

class iclient {
public:
	virtual void init(const char* pszHostname, const char* pszUsername) = 0;
	virtual void shutdown() = 0;
	virtual bool is_shutdown() = 0;
};

extern "C" {
	ENL_EXPORT iclient* client_dll_init();
	ENL_EXPORT void client_dll_shutdown(iclient* p);
}
