#include "stdafx.h"
#include <efs/efs.h>
#include <print.h>
#include <adler32.h>
#include <iomanip>
#include <bitset>
#include <algorithm>
#include <execution>

struct efs_handle {
	const efs* pVolume;
	size_t iStart;
	size_t iCursor;
	size_t nLength;
};

efs::efs(const std::string& path, bool bDontCreate) : m_bReadOnly(true) {
	m_hFile = fopen(path.c_str(), "r+b");
	if (!m_hFile && !bDontCreate) { 
		// Probably doesn't exist
		PRINT_DBG("EFS: volume doesn't exist, creating...");
		m_hFile = fopen(path.c_str(), "w+b");
		ASSERT(m_hFile);
		m_bReadOnly = false;
		return;
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
	m_hashmethod = (efs_hashmethod)hdr.iHashMethod;
	m_offHashtable = hdr.offTableHeader;
	load_ht();
	PRINT("EFS: volume " << path << " mounted!");
}

efs::~efs() {
	if (m_hFile) {
		fflush(m_hFile);
		fclose(m_hFile);
	}
}

size_t efs::size(uint32_t hash) {
	if (!m_hFile) return 0;
	if (!file_exists(hash)) return 0;
	const auto& h = find_file_by_hash(hash);
	return h.size;
}

void efs::read(uint32_t hash, void * buf, size_t size) {
	if (!m_hFile) return;
	if (!file_exists(hash)) return;
	const auto& h = find_file_by_hash(hash);
	if (h.offset == -1) return;
	auto off = h.offset;

	fseek(m_hFile, off, SEEK_SET);
	size_t nRead = (h.size < size) ? h.size : size;
	size_t nWritten = fread(buf, 1, nRead, m_hFile);
	if (nWritten != h.size) {
		PRINT_DBG("EFS: read failure");
	}
}

void efs::begin_write() {
	if (!m_hFile) return;
	if (m_bReadOnly) return;
	// Reserve space for header
	// Fill in whatever fields we know
	PRINT("EFS [1/3] Preparing volume");
	efs_header hdr;
	memset(&hdr, 0, sizeof(hdr));
	fseek(m_hFile, 0, SEEK_SET);
	fwrite(&hdr, sizeof(hdr), 1, m_hFile);
	PRINT("EFS [2/3] Writing file data");
}

uint32_t efs::write(void * buf, size_t size) {
	if (!m_hFile) return 0;
	if (m_bReadOnly) return 0;
	size_t nWritten = 0;
	auto hash = adler32().apply((uint8_t*)buf, size).result();

	// Discard empty files
	if (hash == adler32::empty_file)
		return 0;

	// Go to the volume's end and write the data out
	fseek(m_hFile, 0, SEEK_END);
	auto offFileStart = ftell(m_hFile);
	nWritten = fwrite(buf, 1, size, m_hFile);
	if (nWritten != size) {
		PRINT_ERR("EFS: Failed to write file to the volume!!!");
	}
	ASSERT(nWritten == size);

	efs_hashtable_entry hte;
	hte.nFileSiz = size;
	hte.nHash = hash;
	hte.offFileStart = offFileStart;
	m_entries.push_back(hte);
	return hash;
}

void efs::end_write() {
	if (!m_hFile) return;
	if (m_bReadOnly) return;
	size_t nWritten = 0;
	// Write hash table header
	m_offHashtable = ftell(m_hFile);
	efs_hashtable_header hthdr;
	hthdr.nSignature = efs_ht_signature;
	hthdr.nCurEntries = m_entries.size();
	nWritten = fwrite(&hthdr, 1, sizeof(hthdr), m_hFile);
	ASSERT(nWritten == sizeof(hthdr));

	PRINT("EFS [3/3] Writing " << m_entries.size() << " entries");
	nWritten = fwrite(&m_entries[0], sizeof(efs_hashtable_entry), m_entries.size(), m_hFile);
	fflush(m_hFile);
	ASSERT(nWritten == m_entries.size());

	efs_header hdr;
	hdr.nSignature = efs_signature;
	hdr.iHashMethod = EFS_HASH_ADLER32;
	hdr.offTableHeader = m_offHashtable;
	memset(&hdr.reserved, 0, sizeof(hdr.reserved));
	fseek(m_hFile, 0, SEEK_SET);
	nWritten = fwrite(&hdr, 1, sizeof(hdr), m_hFile);
	fflush(m_hFile);
	ASSERT(nWritten == sizeof(hdr));
	PRINT("EFS: finalized volume!");
}

void efs::list_files() {
	if (!m_hFile) return;
	PRINT("Number of files: " << m_hash.size());
	PRINT("ID\t\t\tOffset\tSize");
	for (const auto& hash : m_hash) {
		PRINT(std::hex << hash.value << "\t\t" << hash.offset << "\t\t" << hash.size);
	}
}

bool efs::file_exists(uint32_t hash) {
	const auto& it = std::find_if(std::execution::par, m_hash.begin(), m_hash.end(), [&hash](const auto& e) {
		return e.value == hash;
	});
	return it != m_hash.end();
}

const efs_hash& efs::find_file_by_hash(uint32_t hash) {
	const auto& it = std::find_if(std::execution::par, m_hash.begin(), m_hash.end(), [&hash](const auto& e) {
		return e.value == hash;
	});
	const efs_hash& h = *it;
	return h;
}

void efs::load_ht() {
	size_t nRead = 0;
	//PRINT_DBG("EFS: loading hash table");
	efs_hashtable_header hthdr;
	fseek(m_hFile, m_offHashtable, SEEK_SET);
	fread(&hthdr, sizeof(hthdr), 1, m_hFile);
	if (hthdr.nSignature != efs_ht_signature) {
		PRINT_ERR("EFS: HT has bad signature. Volume is considered empty.");
		return;
	}
	constexpr size_t nBatchReadCount = 64; // Increase this if better I/O perf is needed and memory usage is not a concern
	efs_hashtable_entry pEntries[nBatchReadCount];
	size_t nEntriesToRead = hthdr.nCurEntries;
	size_t nEntries = 0;
	while(nEntriesToRead) {
		size_t nEntriesToReadThisRound = nEntriesToRead >= nBatchReadCount ? nBatchReadCount : nEntriesToRead;
		// Load no more than `nBatchReadCount` entries at once
		nRead = fread(pEntries, sizeof(efs_hashtable_entry), nEntriesToReadThisRound, m_hFile);
		if (nRead < nEntriesToReadThisRound && feof(m_hFile)) {
			nEntriesToRead = 0;
			nEntriesToReadThisRound = nRead;
		} else {
			nEntriesToRead -= nEntriesToReadThisRound;
		}
		for (size_t j = 0; j < nEntriesToReadThisRound; j++) {
			efs_hash h;
			h.offset = pEntries[j].offFileStart;
			h.size = pEntries[j].nFileSiz;
			h.value = pEntries[j].nHash;
			m_hash.push_back(h);
		}
	}

	std::sort(m_hash.begin(), m_hash.end(), [](const auto& lhs, const auto& rhs) {
		return lhs.value < rhs.value;
	});
}
