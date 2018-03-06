#include "stdafx.h"
#include "material.h"
#include "qc_parser.h"
#include "renderer.h"

material::material(const mdlc::qc_parser& qcp)
{
	if (!qcp.is_cmd("shader"))
	{
		PRINT_ERR("No shader defined in material ");
		ASSERT(0);
	}
	
	m_qcp = qcp;

	m_iszShader = qcp.get_string("shader");
	m_iShader = -1;
}

material::material(const material & other)
{
	m_qcp = other.m_qcp;
	m_iShader = other.m_iShader;
	m_iszShader = other.m_iszShader;
	memcpy(m_aiTextures, other.m_aiTextures, sizeof(uint32_t) * MAT_TEX_MAX);
}

int material::get_shader()
{
	if (m_iShader == -1) {
		m_iShader = gpGlobals->pRenderer->get_shader_program_index(m_iszShader);
	}
	return m_iShader;
}

void material::set_texture(mat_tex_index iType, uint32_t iTex)
{
	m_aiTextures[iType] = iTex;
}
