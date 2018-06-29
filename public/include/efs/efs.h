#pragma once

#include <string>
#include <cstdint>
#include <enl/platform.h>
#include <cstdio>

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
// offset of the first and last hash tables.
//
// The hash tables contain at most 65535 entries and they must begin
// on sector boundary. Files also start on sector boundaries.
// A hash table entry specifies the files's offset and it's length.
//
// There is a primary table and more secondary tables. The primary
// table starts immediately after the EFS header.
//
// Once a hash table is full, another must be made. Every table has
// links to the next one. If this link's value is zero then this
// specific table is not full yet and is writeable.
// Hash tables are append only and once they are full they cannot
// be further written to.
//
// Since assets usually change while the game is in development
// and assets stored in a volume cannot be changed anymore,
// it's advised to either
// - recreate these volumes containing only the most recent version
// of each file
// - only use EFS to distribute to customers

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
	uint64_t offFirstTableHeader;
	uint64_t offLastTableHeader;
	uint8_t reserved[488];
} PACKED;
END_PACK

START_PACK
struct efs_hashtable_header {
	uint32_t nSignature;
	uint16_t nCurEntries;
	uint16_t nMaxEntries;
	uint64_t offNextTable;
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

// Hash tables and files must start on sector boundary!
// What we call sectors here are actually virtual sectors since
// we can't control where we write on the actual hardware.
// But we can assume that modern filesystems start files on
// sector boundaries, so every 512th byte offset marks the
// beginning of a sector.
// Offset mask to use: 0x1FF

constexpr uint64_t efs_signature = 0x0000000000000000;
constexpr size_t efs_hashtable_min_entry_count = (512 - sizeof(efs_hashtable_header)) / sizeof(efs_hashtable_entry);
// Space reserved for primary hash table
constexpr size_t efs_primary_ht_reservation = 8192 - sizeof(efs_hashtable_header); // bytes

constexpr uint32_t efs_ht_signature = 0x48544844;

struct efs_handle;

class efs {
public:
	efs(const std::string& path);

	size_t size(uint8_t hash[32]);
	void read(uint8_t hash[32], void* buf, size_t size);
	void write(uint8_t out_hash[32], void* buf, size_t size);

	void list_files();

private:
	bool find_file_by_hash(uint32_t nHash);

	void read_sectors(uint8_t* pBuf, size_t nCount, size_t iOff);
	void write_sectors(uint8_t* pBuf, size_t nCount, size_t iOff);

	void create_volume();
	size_t create_table();

private:
	FILE* m_hFile;
	size_t m_offFirstHastable;
	size_t m_offLastHastable;
	enum efs_hash_method m_hashmethod;
};
