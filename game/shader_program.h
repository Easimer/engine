#pragma once

#include "shader.h"

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
};