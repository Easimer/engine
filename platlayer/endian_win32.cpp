#include "stdafx.h"
#include <plat/endian.h>
#include <print.h>

namespace plat {
	static int bLittleEndian = -1;

	bool little_endian() {
		if (bLittleEndian == -1) {
			short n = 0x1;
			char* p = (char*)&n;
			bLittleEndian = p[0] == 1;
			PRINT_DBG("CPU is " << (bLittleEndian ? "little-endian" : "big-endian"));
		}
		return bLittleEndian;
	}
}
