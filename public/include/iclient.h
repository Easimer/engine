#pragma once

#include <enl/platform.h>

class iclient {
public:
	virtual ~iclient() {}
	// Init module
	virtual void init(const char* pszHostname, const char* pszUsername) = 0;
	// Free module
	virtual void shutdown() = 0;
	// Does the module want to shut down
	virtual bool is_shutdown() = 0;
	// Request launcher to load server.dll
	virtual bool request_server() = 0;
};

extern "C" {
	ENL_EXPORT iclient* client_dll_init();
	ENL_EXPORT void client_dll_shutdown(iclient* p);
}
