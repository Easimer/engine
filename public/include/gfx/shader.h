#pragma once

#include <memory>

namespace gfx {

	enum shader_type {
		SHADER_T_VERTEX,
		SHADER_T_FRAGMENT,
		SHADER_T_GEOMETRY
	};

	class shader {
	public:
		shader();
		shader(const char* szFilename, shader_type iType);
		~shader();
		void operator=(const shader& other) = delete;
		shader(const shader& other) = delete;

		shader_type get_type() { return m_iType; }
		uint32_t get_id() { return m_iID; }

		void set_code(const char* szCode, shader_type iType);

		operator bool() {
			return !m_bBadShader;
		}

	private:
		void print_err();

		bool upload_source(const char* szSource);

	private:
		shader_type m_iType;
		uint32_t m_iID;
		bool m_bBadShader;
	};

	using shared_shader = std::shared_ptr<shader>;
}
