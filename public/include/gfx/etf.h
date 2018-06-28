#pragma once

#include <cstdint>
#include <memory>
#include <enl/platform.h>

namespace gfx::etf {

	enum format : uint32_t {
		ETF_FMT_NONE = 0,
		ETF_FMT_RGB888 = 1,
		ETF_FMT_RGBA8888 = 2,
		ETF_FMT_DXT1 = 3,
		ETF_FMT_MAX,
	};

	constexpr uint32_t signature = 0x00000000;
	constexpr uint32_t version = 0x00000001;

	constexpr const char* chunkid_header = "tHDR";
	constexpr const char* chunkid_image = "tDAT";
	constexpr const char* chunkid_parity = "tXOR";
	constexpr const char* chunkid_metadata = "tMET";

	START_PACK
	struct chunk {
		int8_t chunk_id[4];
		uint32_t chunk_length; // Contains the chunk header's and the chunk content's size
	} PACKED;
	END_PACK

	START_PACK
	struct file_header {
		int8_t chunk_id[4];
		uint32_t chunk_length;

		uint32_t signature;
		uint32_t version;

		uint32_t width, height;
		uint32_t format;

		uint32_t checksum; // Adler-32 checksum of the header (0 when calc)
	} PACKED;
	END_PACK

	START_PACK
	struct metadata {
		int8_t chunk_id[4];
		uint32_t chunk_length;
		char key[32];
		char value[0];
	} PACKED;
	END_PACK

	START_PACK
	struct data {
		int8_t chunk_id[4];
		uint32_t chunk_length;
		uint32_t mipmap_level;
		uint32_t checksum; // Adler-32 checksum of the image data (0 when calc)
		uint32_t reserved1;
		uint32_t reserved2;
		uint8_t value[0];
	} PACKED;
	END_PACK

	static_assert(sizeof(chunk) == 8);
	static_assert(sizeof(file_header) == 32);

	enum conversion_result {
		convresult_ok, // OK
		convresult_general, // Generic error code
		convresult_dimensions, // Bad texture dimensions
		convresult_illegal, // Illegal conversion
		convresult_compression_failure, // Compression failure
		convresult_mem, // Memory error
	};

	class etf_writer {
	public:
		etf_writer() : m_width(0), m_height(0),
			m_iformat(ETF_FMT_NONE), m_oformat(ETF_FMT_NONE),
			m_data(nullptr), m_data_siz(0) {}

		void width(uint32_t v) { m_width = v; }
		void height(uint32_t v) { m_height = v; }
		void input_format(gfx::etf::format v) { m_iformat = v; }
		void output_format(gfx::etf::format v) { m_oformat = v; }
		void image(const uint8_t* const data, size_t n) {
			m_data = data;
			m_data_siz = n;
		}
		size_t size() const noexcept {
			return sizeof(file_header) + (sizeof(chunk) + m_data_siz);
		}

		int write_to_file(const std::string& path);

	protected:
		conversion_result convert_rgb888_dxt1();

	private:
		uint32_t m_width, m_height;
		gfx::etf::format m_iformat, m_oformat;
		const uint8_t* m_data;
		size_t m_data_siz;
	};

	// This implementation can do the following conversions:
	// - Any format		-> itself
	// - Any format		-> NONE
	// - RGB888			-> RGBA8888
	// - RGB888			-> DXT1

	class etf_reader {
	public:
		etf_reader(const std::string& filename);

		uint32_t width() const noexcept { return m_width; }
		uint32_t height() const noexcept { return m_height; }
		gfx::etf::format format() const noexcept { return m_format; }

		std::shared_ptr<uint8_t[]> buf() const noexcept { return m_buf; }
		size_t size() const noexcept { return m_buf_siz; }
	private:
		uint32_t m_width, m_height;
		gfx::etf::format m_format;
		std::shared_ptr<uint8_t[]> m_buf;
		size_t m_buf_siz;
	};

	const char* convresult_str(conversion_result res);
}
