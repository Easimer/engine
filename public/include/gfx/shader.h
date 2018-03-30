#pragma once

namespace gfx {

	enum shader_type {
		SHADER_T_VERTEX,
		SHADER_T_FRAGMENT,
		SHADER_T_GEOMETRY
	};

	class shader {
	public:
		shader(const char* szFilename, shader_type iType);
		~shader();
		void operator=(const shader& other) = delete;
		shader(const shader& other) = delete;

		shader_type get_type() { return m_iType; }
		uint32_t get_id() { return m_iID; }

	private:
		void print_err();

	private:
		shader_type m_iType;
		uint32_t m_iID;
		bool m_bBadShader;
	};

}
