#include "stdafx.h"
#include "material.h"
#include "qc_parser.h"

material::material(const mdlc::qc_parser& qcp)
{
	if (!qcp.is_cmd("shader"))
	{
		PRINT_ERR("No shader defined in material ");
		ASSERT(0);
	}
	
	m_qcp = qcp;

	m_iszShader = qcp.get_string("shader");
}

material::material(const material & other)
{
	m_qcp = other.m_qcp;
	m_iszShader = other.m_iszShader;
	memcpy(m_aiTextures, other.m_aiTextures, sizeof(uint32_t) * MAT_TEX_MAX);
}

const std::string& material::get_shader() const
{
	return m_iszShader;
}

void material::set_texture(mat_tex_index iType, uint32_t iTex)
{
	m_aiTextures[iType] = iTex;
}
