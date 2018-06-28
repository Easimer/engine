#pragma once

#include <array>
#include <string>
#include <qc.h>
#include <gfx/defs.h>
#include <gfx/texture.h>
#include <gfx/shader_program.h>

namespace gfx {

	class material {
	public:
		material() :
		m_bNormalIsRelative(false) {}

		material(const std::string& path);
		material(const mdlc::qc& qcp);

		const mdlc::qc& get_parser() const { return m_qcp; }

		[[deprecated]]
		gfx::shared_shader_program get_shader() {
			return m_shader;
		}

		gfx::shared_shader_program shader() const {
			return m_shader;
		}

		const std::string& get_shader_name() const { return m_iszShader; }

		gfx::shared_tex2d get_texture(texture_type iType) const { return m_aiTextures[iType]; }
		bool normal_is_relative() const noexcept { return m_bNormalIsRelative; }

		void use();

	private:
		mdlc::qc m_qcp;
		std::string m_iszShader;

		gfx::shared_shader_program m_shader;

		std::array<gfx::shared_tex2d, TEX_MAX> m_aiTextures;
		bool m_bNormalIsRelative;
	};

}
