#pragma once

#include <enl/platform.h>
#include <cstdint>

#if defined(PLAT_WINDOWS)
#include <Windows.h>
#elif defined(PLAT_LINUX)
#include <arpa/inet.h>
#endif

#define bswap32(x)                                        \
((((x) & 0x00000000ff000000u) >> 24) | (((x) & 0x0000000000ff0000u) >> 8)        \
| (((x) & 0x000000000000ff00u) << 8) | (((x) & 0x00000000000000ffu) << 24))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define htonll(x)	((bswap32(((uint64_t)x) & 0x00000000FFFFFFFF) << 32) | bswap32(((uint64_t)x) >> 32))
#define ntohll(x)	((bswap32(((uint64_t)x) & 0x00000000FFFFFFFF) << 32) | bswap32(((uint64_t)x) >> 32))
#else
#define htonll(x) x
#define ntohll(x) x
#endif
