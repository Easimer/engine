#pragma once

#include "shader.h"
#include "material.h"
#include "qc_parser.h"

enum shader_tex_type {
	SHADERTEX_DIFFUSE	= 0,
	SHADERTEX_NORMAL	= 1,
	SHADERTEX_SPECULAR	= 2,
	SHADERTEX_OPACITY	= 3,
	SHADERTEX_MAX		= 4
};

class shader_program {
public:
	shader_program(const char* szFilename);
	~shader_program();
	bool link();
	void use();
	void validate();

	void print_err();

	const char* get_name() { return m_szName; }
	const char* get_description() { return m_szDescription; }

	void set_mat_trans(void* pMat);
	void set_mat_view(void* pMat);
	void set_mat_proj(void* pMat);

	uint32_t get_id() const { return m_iID; }

	material load_material(const mdlc::qc_parser& qc);

private:
	uint32_t m_iID;

	shader* m_pShaderVert;
	shader* m_pShaderFrag;

	int m_iUniformMatTrans;
	int m_iUniformMatView;
	int m_iUniformMatProj;
	int m_iUniformTex1;
	int m_iUniformTex2;
	int m_iUniformTex3;
	int m_iUniformTex4;
	int m_iUniformTex5;

	char m_szName[64] = { 0 };
	char m_szDescription[128] = { 0 };

	// Key in .mat file mapped to texture type
	std::map<shader_tex_type, std::string> m_mapTexKey;
	// Default value for key in .mat file
	std::map<shader_tex_type, std::string> m_mapTexDefault;
};