#pragma once

#include <enl/platform.h>

class iserver {
public:
	virtual void init() = 0;
	virtual void shutdown() = 0;
	virtual bool is_shutdown() = 0;
};

extern "C" {
	ENL_EXPORT iserver* server_dll_init();
	ENL_EXPORT void server_dll_shutdown(iserver* p);
}
