#include "stdafx.h"
#include <print.h>
#include <efs/efs.h>
#include <cstdio>
#include <fstream>
#include <iostream>

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
		PRINT("Usage: " << argv[0] << " path_to_volume [files_to_add...]");
		return 1;
	}

	const char* pszVolumePath = argv[1];

	if (argc == 2) {
		efs volume(pszVolumePath, false);
		volume.list_files();
	}
	else if (argc > 2) {
		efs volume(pszVolumePath);
		volume.begin_write();
	
		std::string map_fn = std::string(argv[1]) + ".map";
		std::ofstream file_map(map_fn, std::ios::out | std::ios::trunc);
		for (size_t i = 2; i < argc; i++) {
			uint8_t hash[32];
			std::unique_ptr<uint8_t[]> pFileData;
			size_t nFileSize;
			pFileData = read_file_contents(argv[i], &nFileSize);
			PRINT_DBG("Adding " << argv[i] << " with size=" << nFileSize);
			if (pFileData) {
				volume.write(hash, pFileData.get(), nFileSize);
				file_map << argv[i] << std::endl;
			} else {
				PRINT_DBG("Failed to open and/or read " << argv[i]);
			}
		}
		volume.end_write();
	}
	return 0;
}
