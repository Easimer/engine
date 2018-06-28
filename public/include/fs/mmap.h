#pragma once

#if defined(PLAT_WINDOWS)
#include <Windows.h>
#elif defined(PLAT_LINUX)
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace fs {

#if defined(PLAT_WINDOWS)
	using file_handle = HANDLE;
	const int PROT_NONE = 0;
	const int PROT_EXEC = 1;
	const int PROT_READ = 2;
	const int PROT_WRITE = 4;

	const int MAP_SHARED = 1;
	const int MAP_PRIVATE = 2;
	const int MAP_ANONYMOUS = 4;

	const int OPEN_READONLY = 1;
	const int OPEN_READWRITE = 3;
	const int OPEN_CREATE = 4;

#elif defined(PLAT_LINUX)
	using file_handle = int;

	const int OPEN_READONLY = O_RDONLY;
	const int OPEN_READWRITE = O_RDWR;
	const int OPEN_CREATE = O_CREAT;
#endif

	file_handle open(const char* pszPath, int flags) {
#if defined(PLAT_WINDOWS)
		DWORD dwDesiredAccess = 0;
		DWORD dwCreationDisposition = OPEN_ALWAYS;
		if (flags == OPEN_READONLY) {
			dwDesiredAccess = GENERIC_READ;
		}
		if (flags == OPEN_READWRITE) {
			dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		}
		if (flags == OPEN_CREATE) {
			dwCreationDisposition = CREATE_ALWAYS;
		}
		return CreateFileA(pszPath, dwDesiredAccess, 0, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
#elif defined(PLAT_LINUX)
		return open(pszPath, flags);
#endif
	}

	void close(file_handle h) {
#if defined(PLAT_WINDOWS)
		CloseHandle(h);
#elif defined(PLAT_LINUX)
		close(h);
#endif
	}

	void* mmap(void* addr, size_t length, int prot, int flags, file_handle fd, size_t offset) {
#if defined(PLAT_WINDOWS)
		DWORD flProtect = 0;
		DWORD dwDesiredAccess = 0;
		if (prot == PROT_READ)
			flProtect = PAGE_READONLY;
		if ((prot & PROT_WRITE) && (prot & PROT_READ))
			flProtect = PAGE_READWRITE;
		if ((prot & PROT_EXEC) && (prot & PROT_READ))
			flProtect = PAGE_EXECUTE_READ;
		if ((prot & PROT_READ) && (prot & PROT_WRITE) && (prot & PROT_EXEC))
			flProtect = PAGE_EXECUTE_READWRITE;
		if (flags & MAP_ANONYMOUS) {
			//return VirtualAlloc(addr, length, MEM_COMMIT | MEM_RESERVE, flProtect);
			return nullptr;
		}
		if (!addr) {
			auto hHandle = CreateFileMappingA(fd, nullptr, flProtect, (length >> 32), length & 0xFFFFFFFF, nullptr);
			if (flProtect == PAGE_EXECUTE_READWRITE || flProtect == PAGE_READWRITE)
				dwDesiredAccess = FILE_MAP_ALL_ACCESS;
			if (flProtect == PAGE_EXECUTE_READ || flProtect == PAGE_READONLY)
				dwDesiredAccess = FILE_MAP_READ;
			return MapViewOfFile(hHandle, dwDesiredAccess, 0, 0, 0);
		}
		return nullptr;
#elif defined(PLAT_LINUX)
		return ::mmap(addr, length, prot, flags, fd, offset);
#endif
	}

	int munmap(void* addr, size_t length) {
#if defined(PLAT_WINDOWS)
		if (!addr)
			return -1;
		
		FlushViewOfFile(addr, 0);
		// NOTE: UnmapViewOfFile automatically closes the file mapping handle created in mmap
		if (UnmapViewOfFile(addr))
			return 0;
		else
			return -1;
#elif defined(PLAT_LINUX)
		return ::munmap(addr, length);
#endif
	}
}
