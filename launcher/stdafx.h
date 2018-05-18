#pragma once

#include <enl/platform.h>
#include <enl/assert.h>
#include <enl/cmdline.h>
#if defined(PLAT_WINDOWS)
#include <Windows.h>
#elif defined(PLAT_LINUX)
#include <dlfcn.h>
#endif