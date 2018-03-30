#include "stdafx.h"
#include <gfx/shader.h>

#include "glad/glad.h"
#include <assert_opengl.h>

#include <string>
#include <sstream>
#include <fstream>

using namespace gfx;

shader::shader(const char * szFilename, shader_type iType)
{
	std::ifstream hFile;
	std::stringstream sszSource;
	std::string szSource;
	const char* pszSource;
	int iSuccess;
	GLushort iShaderType;

	m_bBadShader = false;

	hFile.open(szFilename);
	if (hFile.fail())
	{
		PRINT_ERR("shader::shader: cannot open '" << szFilename << "', reason: " << strerror(errno));
		m_bBadShader = true;
		return;
	}

	sszSource << hFile.rdbuf();
	szSource = sszSource.str();
	pszSource = szSource.c_str();

	switch (iType)
	{
	case SHADER_T_VERTEX:
		iShaderType = GL_VERTEX_SHADER;
		break;
	case SHADER_T_FRAGMENT:
		iShaderType = GL_FRAGMENT_SHADER;
		break;
	case SHADER_T_GEOMETRY:
#if defined(PLAT_WINDOWS)
		iShaderType = 0x8DD9;  // HACKHACK: GL_GEOMETRY_SHADER is not defined in Windows headers
#else
		iShaderType = GL_GEOMETRY_SHADER;
#endif
		break;
	}

	m_iID = glCreateShader(iShaderType); ASSERT_OPENGL();
	glShaderSource(m_iID, 1, &pszSource, NULL); ASSERT_OPENGL();
	glCompileShader(m_iID); ASSERT_OPENGL();

	glGetShaderiv(m_iID, GL_COMPILE_STATUS, &iSuccess);

	if (!iSuccess)
	{
		print_err();
		m_bBadShader = true;
	}
	else
	{
		PRINT_DBG("Shader " << szFilename << " has compiled! ID: " << m_iID);
	}
}

shader::~shader()
{
	RESTRICT_THREAD_RENDERING;
	glDeleteShader(m_iID); //ASSERT_OPENGL();
}

void shader::print_err()
{
	char szMsg[512];
	glGetShaderInfoLog(m_iID, 512, NULL, szMsg);
	PRINT_ERR("Shader compilation failed: " << szMsg);
}
