#include "stdafx.h"
#include <fstream>
#include <gfx/material.h>
#include <gfx/gfx.h>

using namespace gfx;

gfx::material::material(const std::string & path) : material(mdlc::qc(std::ifstream(path))) {
}

material::material(const mdlc::qc& qcp)
{
	if (!qcp.is_cmd("shader"))
	{
		PRINT_ERR("No shader defined in material ");
		ASSERT(0);
	}

	m_qcp = qcp;

	m_iszShader = qcp.at<std::string>("shader");
	if (qcp.count("normal_is_relative"))
		m_bNormalIsRelative = qcp.at<bool>("normal_is_relative");
	else
		m_bNormalIsRelative = false;

	auto iShader = gpGfx->get_shader_program_index(m_iszShader);
	auto pShader = gpGfx->get_shader(iShader);
	m_shader = pShader;

	for (size_t i = 0; i < (size_t)TEX_MAX; i++) {
		// Key for the texture
		auto key = pShader->material_key((texture_type)i);
		std::string path;
		if (qcp.count(key)) {
			path = qcp.at<std::string>(key);
		} else {
			path = pShader->material_default((texture_type)i);
		}
		m_aiTextures[i] = gpGfx->load_texture(path);
	}
}

void material::use() {
	for (size_t i = 0; i < (size_t)TEX_MAX; i++) {
		if (m_aiTextures[i]) {
			m_aiTextures[i]->bind(i);
		}
	}
	m_shader->set_bool("bNormalIsRelative", m_bNormalIsRelative);
}
