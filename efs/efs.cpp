#include "stdafx.h"
#include <efs/efs.h>
#include <print.h>
#include <adler32.h>

struct efs_handle {
	const efs* pVolume;
	size_t iStart;
	size_t iCursor;
	size_t nLength;
};

efs::efs(const std::string& path) {
	m_hFile = fopen(path.c_str(), "r+");
	if (!m_hFile) { 
		// Probably doesn't exist
		m_hFile = fopen(path.c_str(), "w+");
		ASSERT(m_hFile);
		create_volume();
	}
	if (!m_hFile)
		return;
	fseek(m_hFile, 0, SEEK_SET);

	efs_header hdr;
	fread(&hdr, sizeof(hdr), 1, m_hFile);
	if (hdr.nSignature != efs_signature) {
		PRINT_ERR("EFS ERROR: volume " << path << " has an invalid signature!");
		fclose(m_hFile);
		return;
	}
	if (hdr.iHashMethod == 0 || hdr.iHashMethod >= EFS_HASH_MAX) {
		PRINT_ERR("EFS ERROR: volume " << path << " uses unsupported hash method!");
		fclose(m_hFile);
		return;
	}
	m_hashmethod = (enum efs_hash_method)hdr.iHashMethod;
	m_offFirstHastable = hdr.offFirstTableHeader;
	m_offLastHastable = hdr.offLastTableHeader;
	PRINT("EFS: volume " << path << " mounted!");
}

size_t efs::size(uint8_t hash[32]) {
	return size_t();
}

void efs::read(uint8_t hash[32], void * buf, size_t size) {
}

void efs::write(uint8_t out_hash[32], void * buf, size_t size) {
	PRINT_DBG("EFS: writing file");

	// Go to the volume's end and write the data out
	fseek(m_hFile, 0, SEEK_END);
	auto offFileStart = ftell(m_hFile);
	fwrite(buf, size, 1, m_hFile);

	auto offVolumeEnd = ftell(m_hFile);

	// Read header, increment entry counter
	// and write back
	efs_hashtable_header hthdr;
	fseek(m_hFile, m_offLastHastable, SEEK_SET);
	auto offTableHeader = ftell(m_hFile);
	fread(&hthdr, sizeof(hthdr), 1, m_hFile);
	ASSERT(hthdr.nSignature == efs_ht_signature);

	while (hthdr.nCurEntries == hthdr.nMaxEntries - 1 && hthdr.offNextTable) {
		PRINT_DBG("EFS: this table is full, jumping to the next one");
		fseek(m_hFile, hthdr.offNextTable, SEEK_SET);
		offTableHeader = ftell(m_hFile);
		fread(&hthdr, sizeof(hthdr), 1, m_hFile);
		ASSERT(hthdr.nSignature == efs_ht_signature);
	}

	hthdr.nCurEntries++;
	if (hthdr.nCurEntries == hthdr.nMaxEntries - 1) {
		PRINT_DBG("EFS: volume hash table has been filled, creating new one");
		hthdr.offNextTable = offVolumeEnd;
		create_table();
	}
	fseek(m_hFile, offTableHeader, SEEK_SET);
	fwrite(&hthdr, sizeof(hthdr), 1, m_hFile);

	// Read hashtable

	std::unique_ptr<efs_hashtable_entry[]> pTable = std::make_unique<efs_hashtable_entry[]>(hthdr.nMaxEntries);
	auto offTableStart = ftell(m_hFile);
	fread(pTable.get(), sizeof(efs_hashtable_entry), hthdr.nMaxEntries, m_hFile);

	efs_hashtable_entry* pEntry = &pTable[hthdr.nCurEntries];

	switch (m_hashmethod) {
	case EFS_HASH_ADLER32:
		pEntry->nHash0 = adler32().apply((uint8_t*)buf, size).result();
		break;
	}
	pEntry->offFileStart = offFileStart;
	pEntry->nFileSiz = size;

	// Write table

	fseek(m_hFile, offTableStart, SEEK_SET);
	fwrite(pTable.get(), sizeof(efs_hashtable_entry), hthdr.nMaxEntries, m_hFile);

	fflush(m_hFile);
}

void efs::list_files() {
	efs_hashtable_header hthdr;
	fseek(m_hFile, m_offLastHastable, SEEK_SET);
	auto offTableHeader = ftell(m_hFile);
	fread(&hthdr, sizeof(hthdr), 1, m_hFile);
	ASSERT(hthdr.nSignature == efs_ht_signature);
	{
		std::unique_ptr<efs_hashtable_entry[]> pTable = std::make_unique<efs_hashtable_entry[]>(hthdr.nMaxEntries);
		fread(pTable.get(), sizeof(efs_hashtable_entry), hthdr.nMaxEntries, m_hFile);
		for (size_t i = 0; i < hthdr.nMaxEntries; i++) {
			if (pTable[i].nFileSiz) {
				PRINT_DBG(pTable[i].nHash0 << " size: " << pTable[i].nFileSiz << " bytes");
			}
		}
	}

	while (hthdr.offNextTable) {
		PRINT_DBG("EFS: this table is full, jumping to the next one");
		fseek(m_hFile, hthdr.offNextTable, SEEK_SET);
		offTableHeader = ftell(m_hFile);
		fread(&hthdr, sizeof(hthdr), 1, m_hFile);
		ASSERT(hthdr.nSignature == efs_ht_signature);
		{
			std::unique_ptr<efs_hashtable_entry[]> pTable = std::make_unique<efs_hashtable_entry[]>(hthdr.nMaxEntries);
			fread(pTable.get(), sizeof(efs_hashtable_entry), hthdr.nMaxEntries, m_hFile);
			for (size_t i = 0; i < hthdr.nMaxEntries; i++) {
				if (pTable[i].nFileSiz) {
					PRINT_DBG(pTable[i].nHash0 << " size: " << pTable[i].nFileSiz << " bytes");
				}
			}
		}
	}
}

bool efs::find_file_by_hash(uint32_t nHash) {
	return false;
}

void efs::read_sectors(uint8_t * pBuf, size_t nCount, size_t iOff) {
}

void efs::write_sectors(uint8_t * pBuf, size_t nCount, size_t iOff) {
}

void efs::create_volume() {
	PRINT_DBG("EFS: [1/3] formatting new volume");
	efs_header hdr;
	hdr.nSignature = efs_signature;
	hdr.offFirstTableHeader = 512;
	hdr.iHashMethod = EFS_HASH_ADLER32;
	hdr.offLastTableHeader = 512;
	fwrite(&hdr, sizeof(hdr), 1, m_hFile);
	PRINT_DBG("EFS: [2/3] written header");

	fseek(m_hFile, 512, SEEK_SET);
	efs_hashtable_header hthdr;
	hthdr.nSignature = efs_ht_signature;
	hthdr.nCurEntries = 0;
	hthdr.nMaxEntries = efs_primary_ht_reservation / sizeof(efs_hashtable_entry);
	hthdr.offNextTable = 0;
	
	fwrite(&hthdr, sizeof(hthdr), 1, m_hFile);

	uint64_t nZero = 0;
	fseek(m_hFile, efs_primary_ht_reservation, SEEK_CUR);
	fwrite(&nZero, sizeof(nZero), 1, m_hFile);

	fflush(m_hFile);
	PRINT_DBG("EFS: [3/3] written primary hash table");
}

size_t efs::create_table() {
	fseek(m_hFile, 0, SEEK_END);
	size_t ret = ftell(m_hFile);
	efs_hashtable_header hdr;
	hdr.nCurEntries = 0;
	hdr.nMaxEntries = efs_primary_ht_reservation / sizeof(efs_hashtable_entry);
	hdr.offNextTable = 0;
	fwrite(&hdr, sizeof(hdr), 1, m_hFile);
	return ret;
}
