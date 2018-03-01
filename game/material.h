#pragma once

#include <string>
#include "qc_parser.h"

class material {
public:
	material(const mdlc::qc_parser& qcp);

private:
	std::string m_iszShader;
	uint32_t m_iTextureDiffuse;
	uint32_t m_iTextureNormal;
	uint32_t m_iTextureSpecular;
	uint32_t m_iTextureOpacity;
};
