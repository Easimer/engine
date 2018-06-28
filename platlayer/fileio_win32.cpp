#include "stdafx.h"
#include <plat/fileio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace plat {
	struct file_handle {
		HANDLE hFile;
	};

	struct memory_map {
		HANDLE hMapping;
		void* pView;
		size_t nSize;
	};

	file_handle* open(const char* pszFilename, int flags) {
		DWORD dwDesiredAccess = 0;
		DWORD dwCreationDisposition = 0;

		file_handle* pHandle = new file_handle;
		pHandle->hFile = CreateFileA(pszFilename, dwDesiredAccess, 0, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
		return pHandle;
	}

	void close(file_handle* h) {
		CloseHandle(h->hFile);
		delete h;
	}

	inline DWORD translate_memprot_flags(int prot) {
		if (prot == PROT_READ)
			return PAGE_READONLY;
		if (prot == PROT_WRITE)
			return PAGE_READWRITE;
		if ((prot & PROT_READ) && (prot & PROT_WRITE))
			return PAGE_READWRITE;
		if ((prot & PROT_READ) && (prot & PROT_EXEC))
			return PAGE_EXECUTE_READ;
		if ((prot & PROT_READ) && (prot & PROT_WRITE) && (prot & PROT_EXEC))
			return PAGE_EXECUTE_READWRITE;
		return 0;
	}

	void* virtual_alloc(void* addr, size_t length, int prot) {
		DWORD flProtect = translate_memprot_flags(prot);

		ASSERT(flProtect);

		return VirtualAlloc(addr, length, MEM_COMMIT | MEM_RESERVE, flProtect);
	}

	void virtual_free(void* addr) {
		VirtualFree(addr, 0, MEM_RELEASE);
	}

	memory_map* mmap_file(void* addr, int prot, file_handle* fd) {
		DWORD flProtect = translate_memprot_flags(prot);
		DWORD dwDesiredAccess = 0;

		ASSERT(flProtect);

		auto handle = CreateFileMappingA(fd->hFile, nullptr, flProtect, 0, 0, nullptr);
		
		if(flProtect == PAGE_READONLY)
			dwDesiredAccess = FILE_MAP_READ;
		if (flProtect == PAGE_READWRITE)
			dwDesiredAccess = FILE_MAP_WRITE;
		if (flProtect == PAGE_EXECUTE_READWRITE)
			dwDesiredAccess = FILE_MAP_ALL_ACCESS;

		void* pView = MapViewOfFile(handle, dwDesiredAccess, 0, 0, 0);

		memory_map* pMap = new memory_map;
		pMap->pView = pView;
		pMap->hMapping = handle;

		MEMORY_BASIC_INFORMATION info;
		VirtualQuery(pView, &info, sizeof(info));

		pMap->nSize = info.RegionSize;
		return pMap;
	}

	void munmap_file(memory_map* mmap) {
		UnmapViewOfFile(mmap->pView);
		CloseHandle(mmap->hMapping);
		delete mmap;
	}

	void* mmap_addr(memory_map* mmap) {
		return mmap->pView;
	}

	size_t mmap_len(memory_map* mmap) {
		return mmap->nSize;
	}

}

