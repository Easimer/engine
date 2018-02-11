#pragma once

#define MEMSTAT_MAX_BLOCKS 16384

struct memstat {
	void* pBlock;
	size_t nSize;
};

void memstat_print();
void memstat_init();

extern bool memstat_enabled;
