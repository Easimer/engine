#pragma once

#include <string>
#include <qc.h>

namespace gfx {

	// HACKHACKHACK: keep this in sync with shader_tex_type
	enum mat_tex_index {
		MAT_TEX_DIFFUSE = 0,
		MAT_TEX_NORMAL = 1,
		MAT_TEX_SPECULAR = 2,
		MAT_TEX_OPACITY = 3,
		MAT_TEX_MAX = 4
	};

	class material {
	public:
		material() :
		m_iShader(-1) {}

		material(const mdlc::qc& qcp);
		material(const material& other);

		const mdlc::qc& get_parser() const { return m_qcp; }

		int get_shader();

		const std::string& get_shader_name() const { return m_iszShader; }

		void set_texture(mat_tex_index iType, uint32_t iTex);

		uint32_t get_texture(mat_tex_index iType) const { return m_aiTextures[iType]; }

	private:
		mdlc::qc m_qcp;
		std::string m_iszShader;

		int m_iShader;

		uint32_t m_aiTextures[MAT_TEX_MAX];
	};

}
