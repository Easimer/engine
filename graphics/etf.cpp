#include "stdafx.h"
#include <gfx/etf.h>
#include <adler32.h>
#include <fstream>
#include "glad/glad.h"
#include <plat/fileio.h>

#define STRINTEG(str) (uint32_t)((str[0] << 24) | (str[1] << 16) | (str[2] << 8) | (str[3]))

int gfx::etf::etf_writer::write_to_file(const std::string & path) {
	// Do we have image data?
	if (m_data_siz == 0 || m_data == nullptr)
		return -1;

	// Check format conversion parameter sanity
	if (m_iformat != m_oformat) {
		if (m_iformat == ETF_FMT_RGB888 && m_oformat == ETF_FMT_DXT1) {
			convert_rgb888_dxt1();
		} else {
			return -1;
		}
	}

	std::ofstream f(path, std::ios::binary);
	if (f.bad())
		return -1;

	// Prepare file header
	gfx::etf::file_header hdr;
	hdr.chunk_id[0] = chunkid_header[0];
	hdr.chunk_id[1] = chunkid_header[1];
	hdr.chunk_id[2] = chunkid_header[2];
	hdr.chunk_id[3] = chunkid_header[3];
	hdr.chunk_length = sizeof(file_header);

	hdr.signature = gfx::etf::signature;
	hdr.version = gfx::etf::version;
	hdr.width = m_width;
	hdr.width = m_height;
	hdr.format = (uint32_t)m_oformat;

	hdr.checksum = 0;
	adler32 chksum_hdr;
	chksum_hdr.apply(&hdr, 1);
	hdr.checksum = chksum_hdr.result();
	f.write((char*)&hdr, sizeof(hdr));
	
	if (m_oformat != ETF_FMT_NONE) {
		// Prepare image data chunk header
		data dat;
		dat.chunk_id[0] = chunkid_image[0];
		dat.chunk_id[1] = chunkid_image[1];
		dat.chunk_id[2] = chunkid_image[2];
		dat.chunk_id[3] = chunkid_image[3];

		dat.chunk_length = sizeof(chunk) + m_data_siz;

		adler32 chksum_dat;
		chksum_dat.apply(m_data, m_data_siz);
		dat.checksum = chksum_dat.result();
		dat.mipmap_level = 0;
		f.write((char*)&dat, sizeof(dat));

		// Write image data
		f.write((char*)m_data, m_data_siz);
	}

	// Close file
	f.close();

	return 0;
}

gfx::etf::conversion_result gfx::etf::etf_writer::convert_rgb888_dxt1() {
	GLuint tex;
	GLint compressed = GL_FALSE;
	GLint size = 0;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
	// Check if texture got compressed
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
	if (!compressed) {
		return convresult_compression_failure;
	}

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size);
	uint8_t* pBuf = new uint8_t[size];
	if (!pBuf) {
		return convresult_mem;
	}

	glGetCompressedTexImage(GL_TEXTURE_2D, 0, pBuf);

	m_data = pBuf;
	m_data_siz = size;

	return convresult_ok;
}

const char* gfx::etf::convresult_str(conversion_result res) {
	switch (res) {
	case convresult_ok: return "OK";
	case convresult_general: return "General error";
	case convresult_dimensions: return "Bad image size";
	case convresult_illegal: return "Illegal conversion";
	case convresult_compression_failure: return "Compression failure";
	case convresult_mem: return "Memory failure (low memory?)";
	}
	return "Unknown error";
}

gfx::etf::etf_reader::etf_reader(const std::string & filename) {
	auto hFile = plat::open(filename.c_str(), plat::OPEN_READONLY);
	ASSERT(hFile);
	auto hMapping = plat::mmap_file(nullptr, plat::PROT_READ, hFile);
	ASSERT(hMapping);
	uint8_t* pBuf = (uint8_t*)plat::mmap_addr(hMapping);
	size_t nSize = plat::mmap_len(hMapping);
	ASSERT(pBuf);

	while (pBuf < pBuf + nSize) {
		chunk* pChunk = (chunk*)pBuf;
		file_header* pFileHeader = (file_header*)pBuf;
		adler32 chk;
		uint32_t chkval;
		switch (STRINTEG(pChunk->chunk_id)) {
		case STRINTEG(chunkid_header):
		{
			PRINT_DBG("Header detected");
			chkval = 0;
			std::swap(chkval, pFileHeader->checksum);
			chk.apply(pFileHeader, 1);
			if (chkval != chk.result()) {
				PRINT_DBG("Bad header");
				ASSERT(0);
			}

			m_width = pFileHeader->width;
			m_height = pFileHeader->height;
			break;
		}
		default:
		{
			PRINT_DBG("Unknown chunk ID");
			ASSERT(0);
		}
		}
	}

	plat::munmap_file(hMapping);
	plat::close(hFile);
}
