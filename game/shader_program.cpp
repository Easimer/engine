#include "stdafx.h"
#include "shader_program.h"
#include "shader.h"
#include "qc_parser.h"

#include "glad/glad.h"
#include <assert_opengl.h>

shader_program::shader_program(const char * szFilename) :
	m_pShaderVert(NULL),
	m_pShaderFrag(NULL)
{
	RESTRICT_THREAD_RENDERING;
	mdlc::qc_parser parser(szFilename);
	
	
	if (parser.is_cmd("vertexshader") && parser.is_cmd("fragmentshader"))
	{
		PRINT_DBG("vertex and frag shader defined");
	}
	else
	{
		PRINT_ERR("Vertex and/or fragment shader missing from shader program!");
		return;
	}

	if (parser.is_cmd("name"))
	{
		strncpy(m_szName, parser.get_string("name").c_str(), 64);
	}
	else
	{
		PRINT_ERR("Shader has undefined name!");
		return;
	}
	if (parser.is_cmd("description"))
	{
		strncpy(m_szDescription, parser.get_string("description").c_str(), 128);
	}

	m_iID = glCreateProgram(); ASSERT_OPENGL();

	auto vert = parser.get_string("vertexshader");
	auto frag = parser.get_string("fragmentshader");

	m_pShaderVert = new shader(vert.c_str(), SHADER_T_VERTEX);
	ASSERT(m_pShaderVert);
	m_pShaderFrag = new shader(frag.c_str(), SHADER_T_FRAGMENT);
	ASSERT(m_pShaderFrag);

	glAttachShader(m_iID, m_pShaderVert->get_id());
	glAttachShader(m_iID, m_pShaderFrag->get_id());

	if (!link()) {
		PRINT_ERR("Link of shader " << m_szName << " has failed!");
		ASSERT(0);
		return;
	}

	glUseProgram(m_iID); ASSERT_OPENGL();

	if (parser.is_cmd("uniformtrans"))
	{
		m_iUniformMatTrans = glGetUniformLocation(m_iID, parser.get_string("uniformtrans").c_str()); ASSERT_OPENGL();
		ASSERT(m_iUniformMatTrans != -1);
	}
	else
	{
		PRINT_ERR("uniformtrans undefined for shader " << m_szName);
		return;
	}

	if (parser.is_cmd("uniformview"))
	{
		m_iUniformMatView = glGetUniformLocation(m_iID, parser.get_string("uniformview").c_str()); ASSERT_OPENGL();
		ASSERT(m_iUniformMatView != -1);
	}
	else
	{
		PRINT_ERR("uniformview undefined for shader " << m_szName);
		return;
	}

	if (parser.is_cmd("uniformproj"))
	{
		m_iUniformMatProj = glGetUniformLocation(m_iID, parser.get_string("uniformproj").c_str()); ASSERT_OPENGL();
		ASSERT(m_iUniformMatProj != -1);
	}
	else
	{
		PRINT_ERR("uniformproj undefined for shader " << m_szName);
		return;
	}

	m_iUniformTex1 = glGetUniformLocation(m_iID, "tex_tex1"); ASSERT_OPENGL();
	m_iUniformTex2 = glGetUniformLocation(m_iID, "tex_tex2"); ASSERT_OPENGL();
	m_iUniformTex3 = glGetUniformLocation(m_iID, "tex_tex3"); ASSERT_OPENGL();
	m_iUniformTex4 = glGetUniformLocation(m_iID, "tex_tex4"); ASSERT_OPENGL();
	m_iUniformTex5 = glGetUniformLocation(m_iID, "tex_tex5"); ASSERT_OPENGL();

	glUniform1i(m_iUniformTex1, 0); ASSERT_OPENGL();
	glUniform1i(m_iUniformTex2, 1); ASSERT_OPENGL();
	glUniform1i(m_iUniformTex3, 2); ASSERT_OPENGL();
	glUniform1i(m_iUniformTex4, 3); ASSERT_OPENGL();
	glUniform1i(m_iUniformTex5, 4); ASSERT_OPENGL();

	glUseProgram(0);

	if (parser.is_cmd("diffusekey"))
		m_mapTexKey.emplace(SHADERTEX_DIFFUSE, parser.get_string("diffusekey"));
	if (parser.is_cmd("normalkey"))
		m_mapTexKey.emplace(SHADERTEX_NORMAL, parser.get_string("normalkey"));
	if (parser.is_cmd("specularkey"))
		m_mapTexKey.emplace(SHADERTEX_SPECULAR, parser.get_string("specularkey"));
	if (parser.is_cmd("opacitykey"))
		m_mapTexKey.emplace(SHADERTEX_OPACITY, parser.get_string("opacitykey"));

	if (parser.is_cmd("diffusedefault"))
		m_mapTexDefault.emplace(SHADERTEX_DIFFUSE, parser.get_string("diffusedefault"));
	if (parser.is_cmd("normaldefault"))
		m_mapTexDefault.emplace(SHADERTEX_NORMAL, parser.get_string("normaldefault"));
	if (parser.is_cmd("speculardefault"))
		m_mapTexDefault.emplace(SHADERTEX_SPECULAR, parser.get_string("speculardefault"));
	if (parser.is_cmd("opacitydefault"))
		m_mapTexDefault.emplace(SHADERTEX_OPACITY, parser.get_string("opacitydefault"));
}

shader_program::~shader_program()
{
	RESTRICT_THREAD_RENDERING;
	if (m_pShaderVert)
	{
		glDetachShader(m_iID, m_pShaderVert->get_id());
		delete m_pShaderVert;
	}
	if (m_pShaderFrag)
	{
		glDetachShader(m_iID, m_pShaderFrag->get_id());
		delete m_pShaderFrag;
	}
}

bool shader_program::link()
{
	RESTRICT_THREAD_RENDERING;
	int iSuccess;
	glLinkProgram(m_iID); ASSERT_OPENGL();
	glGetProgramiv(m_iID, GL_LINK_STATUS, &iSuccess);
	if (!iSuccess)
	{
		print_err();
		return false;
	}
	validate();

	return true;
}

void shader_program::use()
{
	RESTRICT_THREAD_RENDERING;
	
	glUseProgram(m_iID); ASSERT_OPENGL();
}

void shader_program::validate()
{
	RESTRICT_THREAD_RENDERING;
	char szMsg[512];
	int iSuccess;
	glValidateProgram(m_iID);
	ASSERT_OPENGL();

	glGetProgramiv(m_iID, GL_VALIDATE_STATUS, &iSuccess);
	ASSERT_OPENGL();

	if (!iSuccess)
	{
		glGetProgramInfoLog(m_iID, 512, NULL, szMsg);
		PRINT_ERR("Shader program validation failed: " << szMsg);
		ASSERT(0);
	}
}

void shader_program::print_err()
{
	RESTRICT_THREAD_RENDERING;
	char szMsg[512];
	glGetProgramInfoLog(m_iID, 512, NULL, szMsg);
	PRINT_ERR("Shader program link failed: " << szMsg);
}

void shader_program::set_mat_trans(void * pMat)
{
	RESTRICT_THREAD_RENDERING;
	glUseProgram(m_iID); ASSERT_OPENGL();
	glUniformMatrix4fv(m_iUniformMatTrans, 1, GL_FALSE, (const GLfloat*)pMat); ASSERT_OPENGL();
}

void shader_program::set_mat_view(void * pMat)
{
	RESTRICT_THREAD_RENDERING;
	glUseProgram(m_iID); ASSERT_OPENGL();
	glUniformMatrix4fv(m_iUniformMatView, 1, GL_FALSE, (const GLfloat*)pMat); ASSERT_OPENGL();
}

void shader_program::set_mat_proj(void * pMat)
{
	RESTRICT_THREAD_RENDERING;
	glUseProgram(m_iID); ASSERT_OPENGL();
	glUniformMatrix4fv(m_iUniformMatProj, 1, GL_FALSE, (const GLfloat*)pMat); ASSERT_OPENGL();
}

material shader_program::load_material(const mdlc::qc_parser & qc)
{
	material mat(qc);
	// TODO
	return mat;
}
