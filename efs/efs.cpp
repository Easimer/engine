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
	m_hFile = fopen(path.c_str(), "r+");
	if (!m_hFile && !bDontCreate) { 
		// Probably doesn't exist
		PRINT_DBG("EFS: volume doesn't exist, creating...");
		m_hFile = fopen(path.c_str(), "w+");
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

size_t efs::size(uint8_t hash[32]) {
	if (!m_hFile) return 0;
	const auto& it = std::find_if(std::execution::par, m_hash.begin(), m_hash.end(), [&hash](const auto& e) {
		return memcmp(hash, e, 32 * sizeof(uint8_t)) == 0;
	});
	if (it != m_hash.end()) {
		const efs_hash& h = *it;
		return h.size;
	}
	return 0;
}

void efs::read(uint8_t hash[32], void * buf, size_t size) {
	if (!m_hFile) return;
}

void efs::begin_write() {
	if (!m_hFile) return;
	if (m_bReadOnly) return;
	// Reserve space for header
	// Fill in whatever fields we know
	efs_header hdr;
	memset(&hdr, 0, sizeof(hdr));
	fseek(m_hFile, 0, SEEK_SET);
	fwrite(&hdr, sizeof(hdr), 1, m_hFile);
}

void efs::write(uint8_t out_hash[32], void * buf, size_t size) {
	if (!m_hFile) return;
	if (m_bReadOnly) return;
	size_t nWritten = 0;
	auto hash = adler32().apply((uint8_t*)buf, size).result();

	PRINT_DBG("EFS: writing file");

	// Go to the volume's end and write the data out
	fseek(m_hFile, 0, SEEK_END);
	auto offFileStart = ftell(m_hFile);
	nWritten = fwrite(buf, 1, size, m_hFile);
	if (nWritten != size) {
		PRINT_ERR("EFS: Failed to write file to the volume!!!");
	}
	ASSERT(nWritten == size);
	fflush(m_hFile);

	efs_hashtable_entry hte;
	hte.nFileSiz = size;
	hte.nHash0 = hash;
	hte.offFileStart = offFileStart;
	m_entries.push_back(hte);
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

	nWritten = fwrite(&m_entries[0], sizeof(efs_hashtable_entry), m_entries.size(), m_hFile);
	ASSERT(nWritten == m_entries.size());

	efs_header hdr;
	hdr.nSignature = efs_signature;
	hdr.iHashMethod = EFS_HASH_ADLER32;
	hdr.offTableHeader = m_offHashtable;
	memset(&hdr.reserved, 0, sizeof(hdr.reserved));
	fseek(m_hFile, 0, SEEK_SET);
	nWritten = fwrite(&hdr, 1, sizeof(hdr), m_hFile);
	ASSERT(nWritten == sizeof(hdr));
	PRINT_DBG("Written " << m_entries.size() << " ht");
}

void efs::list_files() {
	if (!m_hFile) return;
	PRINT_DBG("Number of files: " << m_hash.size());
	PRINT_DBG("ID\t\t\tOffset\tSize");
	for (const auto& hash : m_hash) {
		PRINT_DBG(std::hex << std::setfill('0') << std::setw(8) << std::bitset<128>(hash.value) << "\t\t" << hash.offset << "\t\t" << hash.size);
	}
}

size_t efs::find_file_by_hash(uint32_t nHash) {
	if (!m_hFile) return 0;
	return 0;
}

void efs::load_ht() {
	size_t nRead = 0;
	PRINT_DBG("EFS: loading hash table");
	efs_hashtable_header hthdr;
	fseek(m_hFile, m_offHashtable, SEEK_SET);
	fread(&hthdr, sizeof(hthdr), 1, m_hFile);
	PRINT_DBG(m_offHashtable);
	if (hthdr.nSignature != efs_ht_signature) {
		PRINT_ERR("EFS: HT has bad signature. Volume is considered empty.");
		return;
	}
	constexpr size_t nBatchReadCount = 16; // Increase this if better I/O perf is needed and memory usage is not a concern
	efs_hashtable_entry pEntries[nBatchReadCount];
	size_t nEntriesToRead = hthdr.nCurEntries;
	PRINT_DBG(nEntriesToRead);
	size_t nEntries = 0;
	while(nEntriesToRead) {
		size_t nEntriesToReadThisRound = nEntriesToRead >= nBatchReadCount ? nBatchReadCount : nEntriesToRead;
		// Load no more than `nBatchReadCount` entries at once
		nRead = fread(pEntries, sizeof(efs_hashtable_entry), nEntriesToReadThisRound, m_hFile);
		PRINT_DBG("EFS: round: " << nEntriesToReadThisRound << '/' << nRead);
		if (nRead < nEntriesToReadThisRound) {
			PRINT_DBG("EFS: read less entries than wanted: " << nRead);
			nEntriesToRead = 0;
			nEntriesToReadThisRound = nRead;
		} else {
			nEntriesToRead -= nEntriesToReadThisRound;
		}
		for (size_t j = 0; j < nEntriesToReadThisRound; j++) {
			efs_hash h;
			h.offset = pEntries[j].offFileStart;
			h.size = pEntries[j].nFileSiz;
			memcpy(h.value, &pEntries[j].nHash0, 32 * sizeof(uint8_t));
			m_hash.push_back(h);
		}
	}

	std::sort(m_hash.begin(), m_hash.end(), [](const auto& lhs, const auto& rhs) {
		return memcmp(lhs.value, rhs.value, 32 * sizeof(uint8_t)) < 0;
	});
}
