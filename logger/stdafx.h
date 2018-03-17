#pragma once
#include <enl/platform.h>
#if defined(PLAT_WINDOWS)
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#endif
#include <enl/assert.h>
#include "logger_shared.h"