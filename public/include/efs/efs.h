#pragma once

#include <string>
#include <cstdint>
#include <enl/platform.h>
#include <cstdio>
#include <vector>
#include <tree.h>

// ### Engine File System header ##
//
// EFS is a simple filesystem to store game assets.
// 
// It's content addressable: files are addresses by their hashes
// instead of a path. EFS is able to support multiple hash
// functions, but only one in a volume.
//
// A volume is one EFS file. It starts with a header that
// identifies the hash function used in the volume and the
// offset of the hash table.
//
// There is only one such table in a volume. The table
// has a header, which contains the number of entries 
// following the header.

// Hash method enum 0 is reserved
enum efs_hashmethod : uint64_t {
	EFS_HASH_START = 0,
	EFS_HASH_ADLER32,
	EFS_HASH_MAX,
};

START_PACK
struct efs_header {
	uint64_t nSignature;
	uint64_t iHashMethod;
	uint64_t offTableHeader;
	uint8_t reserved[488];
} PACKED;
END_PACK

START_PACK
struct efs_hashtable_header {
	uint32_t nSignature;
	uint64_t nCurEntries;
} PACKED;
END_PACK

START_PACK
struct efs_hashtable_entry {
	uint32_t nHash0;
	uint32_t nHash1;
	uint32_t nHash2;
	uint32_t nHash3;
	uint32_t nHash4;
	uint32_t nHash5;
	uint32_t nHash6;
	uint32_t nHash7;
	uint64_t offFileStart;
	uint64_t nFileSiz;
	uint8_t reserved[16];
} PACKED;
END_PACK

struct efs_hash {
	uint8_t value[32];
	size_t offset;
	size_t size;
};

constexpr uint64_t efs_signature = 0x4553463031455346;
constexpr uint32_t efs_ht_signature = 0x48544844;

struct efs_handle;

class efs {
public:
	efs(const std::string& path, bool bDontCreate = false);

	size_t size(uint8_t hash[32]);
	void read(uint8_t hash[32], void* buf, size_t size);

	void list_files();

	void begin_write();
	void write(uint8_t out_hash[32], void* buf, size_t size);
	void end_write();

private:
	// Returns file data offset
	size_t find_file_by_hash(uint32_t nHash);

	void load_ht();

private:
	FILE* m_hFile;
	size_t m_offHashtable;
	efs_hashmethod m_hashmethod;

	std::vector<efs_hashtable_entry> m_entries;
	std::vector<efs_hash> m_hash;
	bool m_bReadOnly;
};
