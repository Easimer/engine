#pragma once

namespace plat {
	struct file_handle;

	const int OPEN_READONLY		= 1 << 0;
	const int OPEN_READWRITE	= 1 << 1;
	const int OPEN_CREATE		= 1 << 2;

	file_handle* open(const char* pszFilename, int flags);
	void close(file_handle*);

	// Memory protection flags
	const int PROT_NONE = 0 << 0;
	const int PROT_READ = 1 << 0;
	const int PROT_WRITE = 1 << 1;
	const int PROT_EXEC = 1 << 2;

	struct memory_map;

	void* virtual_alloc(void* addr, size_t length, int prot);
	void virtual_free(void* addr);

	memory_map* mmap_file(void* addr, int prot, file_handle* fd);
	void munmap_file(memory_map* mmap);
	void* mmap_addr(memory_map* mmap);
	size_t mmap_len(memory_map* mmap);
}
