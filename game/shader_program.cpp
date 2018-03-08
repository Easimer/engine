#include "stdafx.h"
#include "shader_program.h"
#include "shader.h"
#include "qc_parser.h"
#include "renderer.h"

#include "glad/glad.h"
#include <assert_opengl.h>

shader_program::shader_program(const char * szFilename) :
	m_pShaderVert(NULL),
	m_pShaderFrag(NULL)
{
	RESTRICT_THREAD_RENDERING;
	mdlc::qc_parser parser(szFilename);
	
	
	if (parser.is_cmd("vertex_shader") && parser.is_cmd("fragment_shader"))
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
		PRINT_ERR("Shader has no name!");
		return;
	}
	if (parser.is_cmd("description"))
	{
		strncpy(m_szDescription, parser.get_string("description").c_str(), 128);
	}

	m_iID = glCreateProgram(); ASSERT_OPENGL();

	auto vert = parser.get_string("vertex_shader");
	auto frag = parser.get_string("fragment_shader");

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

	get_uniform_location(parser, "uniform_trans", &m_iUniformMatTrans);
	get_uniform_location(parser, "uniform_view", &m_iUniformMatView);
	get_uniform_location(parser, "uniform_proj", &m_iUniformMatProj);

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

	if (parser.is_cmd("diffuse_key"))
		m_mapTexKey.emplace(SHADERTEX_DIFFUSE, parser.get_string("diffuse_key"));
	if (parser.is_cmd("normal_key"))
		m_mapTexKey.emplace(SHADERTEX_NORMAL, parser.get_string("normal_key"));
	if (parser.is_cmd("specular_key"))
		m_mapTexKey.emplace(SHADERTEX_SPECULAR, parser.get_string("specular_key"));
	if (parser.is_cmd("opacity_key"))
		m_mapTexKey.emplace(SHADERTEX_OPACITY, parser.get_string("opacity_key"));

	if (parser.is_cmd("diffuse_default"))
		m_mapTexDefault.emplace(SHADERTEX_DIFFUSE, parser.get_string("diffuse_default"));
	if (parser.is_cmd("normal_default"))
		m_mapTexDefault.emplace(SHADERTEX_NORMAL, parser.get_string("normal_default"));
	if (parser.is_cmd("specular_default"))
		m_mapTexDefault.emplace(SHADERTEX_SPECULAR, parser.get_string("specular_default"));
	if (parser.is_cmd("opacity_default"))
		m_mapTexDefault.emplace(SHADERTEX_OPACITY, parser.get_string("opacity_default"));
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

bool shader_program::load_material(material & mat)
{
	RESTRICT_THREAD_RENDERING;
	const mdlc::qc_parser& qcp = mat.get_parser();

	for (size_t iTex = 0; iTex < SHADERTEX_MAX; iTex++) {
		auto iszKey = m_mapTexKey[(shader_tex_type)iTex];
		uint32_t iTexObj = 0;
		if (qcp.is_cmd(iszKey.c_str())) {
			iTexObj = gpGlobals->pRenderer->load_texture(qcp.get_string(iszKey.c_str()));
			ASSERT(iTexObj);
			if (!iTexObj)
				return false;
		}
		else {
			auto iszDefaultTex = m_mapTexDefault[(shader_tex_type)iTex];
			iTexObj = gpGlobals->pRenderer->load_texture(iszDefaultTex);
			ASSERT(iTexObj);
			if (!iTexObj)
				return false;
		}
		mat.set_texture((mat_tex_index)iTex, iTexObj);
	}

	return true;
}

void shader_program::use_material(const material & mat)
{
	for (size_t i = 0; i < SHADERTEX_MAX; i++) {
		glActiveTexture(GL_TEXTURE0 + i); ASSERT_OPENGL();
		glBindTexture(GL_TEXTURE_2D, mat.get_texture((mat_tex_index)i)); ASSERT_OPENGL();
	}
}

void shader_program::set_vec3(const std::string & name, const vec3 & v)
{
	RESTRICT_THREAD_RENDERING;
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if(iLoc != -1)
		glUniform3fv(iLoc, 1, v.ptr()); ASSERT_OPENGL();
	
}

void shader_program::set_vec4(const std::string & name, const float * v)
{
	RESTRICT_THREAD_RENDERING;
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if (iLoc != -1)
		glUniform4fv(iLoc, 1, v); ASSERT_OPENGL();
}

void shader_program::set_mat4(const std::string & name, const void * m)
{
	RESTRICT_THREAD_RENDERING;
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if (iLoc != -1)
		glUniformMatrix4fv(iLoc, 1, GL_FALSE, (const GLfloat*)m); ASSERT_OPENGL();
}

void shader_program::set_light1(const shader_point_light& l)
{
}

void shader_program::set_light2(const shader_point_light& l)
{
}

int shader_program::get_uniform_location(const mdlc::qc_parser& qcp, const std::string& name, int* pLocation)
{
	const char* pszCmd = name.c_str();
	if (qcp.is_cmd(pszCmd))
	{
		std::string iszUniform = qcp.get_string(pszCmd).c_str();
		const char* szUniform = iszUniform.c_str();
		int iLoc = glGetUniformLocation(m_iID, szUniform); ASSERT_OPENGL();
		if (pLocation) {
			*pLocation= iLoc;
		}

		if (iLoc == -1) {
			PRINT_ERR("shader_program[" << m_szName << "]: uniform not found: \"" << szUniform << "\"");
		}

		return iLoc;
	}
	else
	{
		PRINT_ERR("shader_program[" << m_szName << "]: missing key: \"" << name << "\"");
		return -1;
	}
}
