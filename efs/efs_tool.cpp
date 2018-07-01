#include "stdafx.h"
#include <print.h>
#include <efs/efs.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <magic/hexstrtoa.h>
#include <plat/endian.h>

std::unique_ptr<uint8_t[]> read_file_contents(const char* pszFilename, size_t* out_nSize) {
	FILE* pSrc = fopen(pszFilename, "r");
	if (pSrc) {
		fseek(pSrc, 0, SEEK_END);
		auto nSrcSize = ftell(pSrc);
		std::unique_ptr<uint8_t[]> pSrcBuf = std::make_unique<uint8_t[]>(nSrcSize);
		fseek(pSrc, 0, SEEK_SET);
		fread(pSrcBuf.get(), 1, nSrcSize, pSrc);
		fclose(pSrc);
		if(out_nSize)
			*out_nSize = (size_t)nSrcSize;
		return pSrcBuf;
	}
	return nullptr;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		PRINT("Usage: " << argv[0] << " path_to_volume [files_to_add... | hashes...]");
		return 1;
	}
	const char* pszVolumePath = argv[1];

	if (argc == 2) {
		efs volume(pszVolumePath, false);
		volume.list_files();
	}
	else if (argc > 2) {
		efs volume(pszVolumePath);
		if (volume.read_only()) {
			// 
			for (size_t i = 2; i < argc; i++) {
				uint8_t hash[4];
				uint32_t hash_value;
				mgc::hexstrtoa(std::string(argv[i]), hash, 4 * sizeof(uint8_t));
				if (plat::little_endian())
					hash_value = (hash[0] << 24) | (hash[1] << 16) | (hash[2] << 8) | (hash[3]);
				else
					hash_value = (hash[3] << 24) | (hash[2] << 16) | (hash[1] << 8) | (hash[0]);
				PRINT("===========================");
				PRINT(std::hex << "File " << std::hex << hash_value);
				PRINT("Size: " << std::dec << volume.size(hash_value) << " bytes");
			}

		} else {
			volume.begin_write();

			std::string map_fn = std::string(argv[1]) + ".map";
			std::ofstream file_map(map_fn, std::ios::out | std::ios::trunc);
			for (size_t i = 2; i < argc; i++) {
				uint32_t hash;
				std::unique_ptr<uint8_t[]> pFileData;
				size_t nFileSize;
				pFileData = read_file_contents(argv[i], &nFileSize);
				PRINT("Adding " << argv[i]);
				if (pFileData) {
					hash = volume.write(pFileData.get(), nFileSize);
					file_map << std::hex << hash << ' ' << argv[i] << std::endl;
				} else {
					PRINT("Failed to open and/or read " << argv[i]);
				}
			}
			volume.end_write();
		}
	}
	return 0;
}
