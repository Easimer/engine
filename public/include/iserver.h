#pragma once

#include <enl/platform.h>
#include <ifsys/imodule.h>

class iserver : public imodule {
public:
	virtual void init() = 0;
	virtual bool shutdown() = 0;
	virtual bool is_shutdown() = 0;
	virtual const char * name() const override = 0;
};

extern "C" {
	ENL_EXPORT iserver* server_dll_init();
	ENL_EXPORT void server_dll_shutdown(iserver* p);
}
