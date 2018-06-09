#pragma once

#include <iostream>
#include <enl/platform.h>
#include <enl/assert.h>
#if defined(PLAT_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(PLAT_LINUX)
#include <dlfcn.h>
#endif
