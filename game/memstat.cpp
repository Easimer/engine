#include "stdafx.h"

bool memstat_enabled = false;

#if defined(PLAT_DEBUG)

#include "memstat.h"
#include <algorithm>

static memstat memoverride_alloc[MEMSTAT_MAX_BLOCKS] = { { NULL, 0 } };

static size_t memstat_counter = 0;
static size_t memstat_counter_max = 0;

static size_t memstat_size_counter = 0;
static size_t memstat_size_counter_max = 0;


void* operator new(size_t nSize)
{
	void* pBlock = malloc(nSize);
	if (!memstat_enabled)
		return pBlock;
	for (size_t i = 0; i < MEMSTAT_MAX_BLOCKS; i++)
	{
		if (memoverride_alloc[i].pBlock == NULL)
		{
			memoverride_alloc[i].pBlock = pBlock;
			memoverride_alloc[i].nSize = nSize;
			memstat_counter++;
			if (memstat_counter > memstat_counter_max)
				memstat_counter_max = memstat_counter;
			memstat_size_counter += nSize;
			if (memstat_size_counter > memstat_size_counter_max)
				memstat_size_counter_max = memstat_size_counter;
			return pBlock;
		}
	}
	return pBlock;
}


void operator delete(void* pBlock)
{
	free(pBlock);
	if (!memstat_enabled)
		return;
	for (size_t i = 0; i < MEMSTAT_MAX_BLOCKS; i++)
	{
		if (memoverride_alloc[i].pBlock == pBlock)
		{
			memstat_counter--;
			memstat_size_counter -= memoverride_alloc[i].nSize;
			memoverride_alloc[i].pBlock = NULL;
			memoverride_alloc[i].nSize = 0;
			return;
			
		}
	}
}

#endif

void memstat_init()
{
#if defined(PLAT_DEBUG)
	memset(memoverride_alloc, 0, sizeof(memoverride_alloc));
#endif
}

void memstat_print()
{
#if defined(PLAT_DEBUG)
	size_t nSum = 0;
	size_t nNum = 0;
	size_t nSumKilo = 0;
	size_t nSumMega = 0;
	size_t nSumGiga = 0;
	for (size_t i = 0; i < MEMSTAT_MAX_BLOCKS; i++)
	{
		if (memoverride_alloc[i].pBlock)
		{
			nNum++;
			nSum += memoverride_alloc[i].nSize;
			//free(memoverride_alloc[i].pBlock);
		}
	}

	nSumKilo = nSum / 1024;
	nSumMega = nSumKilo / 1024;
	nSumGiga = nSumMega / 1024;

	if (nNum == 0)
		return;
	PRINT("============");
	PRINT("Memstat:");
	PRINT("Memory leaked in " << nNum << " places, total " << nSum << " bytes, which is:");
	PRINT(nSumKilo << " KB");
	PRINT(nSumMega << " MB");
	PRINT(nSumGiga << " GB");

	PRINT("============");
	PRINT("Highest total allocation count: " << memstat_counter_max);
	PRINT("Highest total allocated bytes: " << memstat_size_counter_max << " bytes");
	PRINT("============");
#endif
}
