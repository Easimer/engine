#pragma once

#include <enl/platform.h>
#include <ifsys/imodule.h>

class iclient : public imodule {
public:
	virtual ~iclient() {}
	virtual void init() override = 0;
	// Free module
	virtual bool shutdown() = 0;
	// Does the module want to shut down
	virtual bool is_shutdown() = 0;
	// Request launcher to load server.dll
	virtual bool request_server() = 0;

	virtual const char * name() const override = 0;
};

extern "C" {
	ENL_EXPORT iclient* client_dll_init();
	ENL_EXPORT void client_dll_shutdown(iclient* p);
}
