#include "stdafx.h"
#include <gfx/shader_program.h>
#include <gfx/shader.h>
#include <gfx/material.h>
#include <fstream>
#include <math/vector.h>

#include "glad/glad.h"
#include <assert_opengl.h>

using namespace gfx;

gfx::shader_program::shader_program()
{
	m_iID = glCreateProgram(); ASSERT_OPENGL();
}

shader_program::shader_program(const char * szFilename)
{
	m_qc = mdlc::qc(std::ifstream(szFilename));
	
	
	if (m_qc.count("vertex_shader") && m_qc.count("fragment_shader"))
	{
		PRINT_DBG("vertex and frag shader defined");
	}
	else
	{
		PRINT_ERR("Vertex and/or fragment shader missing from shader program!");
		return;
	}

	if (m_qc.count("name"))
	{
		strncpy(m_szName, m_qc.at<std::string>("name").c_str(), 64);
	}
	else
	{
		PRINT_ERR("Shader has no name!");
		return;
	}
	if (m_qc.count("description"))
	{
		strncpy(m_szDescription, m_qc.at<std::string>("description").c_str(), 128);
	}

	m_iID = glCreateProgram(); ASSERT_OPENGL();

	auto vert = m_qc.at<std::string>("vertex_shader");
	auto frag = m_qc.at<std::string>("fragment_shader");

	m_pShaderVert = std::make_shared<shader>(vert.c_str(), SHADER_T_VERTEX);
	ASSERT(m_pShaderVert);
	m_pShaderFrag = std::make_shared<shader>(frag.c_str(), SHADER_T_FRAGMENT);
	ASSERT(m_pShaderFrag);

	m_watchdog_vert = watchdog(vert);
	m_watchdog_frag = watchdog(frag);
	m_sPathVert = vert;
	m_sPathFrag = frag;

	glAttachShader(m_iID, m_pShaderVert->get_id());
	glAttachShader(m_iID, m_pShaderFrag->get_id());

	if (!link()) {
		PRINT_ERR("Link of shader " << m_szName << " has failed!");
		ASSERT(0);
		return;
	}

	setup();
}

shader_program::~shader_program()
{
	// NOTE:
	// Now that a shader program's shader objects
	// are stored in shared_ptrs, we cannot 
	// detach them. We'd need their handle to do
	// that, but since the shared_ptr destructor is called
	// before this destructor, we cannot retrieve
	// that ID.
	// However according to the OpenGL spec, if a shader
	// program is deleted, all of it's attached shaders
	// are automatically detached.
	/*
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
	*/
	if(m_iID)
		glDeleteProgram(m_iID);
}

bool shader_program::link()
{
	int iSuccess;
	glLinkProgram(m_iID); ASSERT_OPENGL();
	glGetProgramiv(m_iID, GL_LINK_STATUS, &iSuccess); ASSERT_OPENGL();
	if (!iSuccess)
	{
		print_err();
		return false;
	}
	validate();
	PRINT_DBG("gfx::shader_program: " << m_iID << " has been linked");
	return true;
}

void shader_program::use()
{
	glUseProgram(m_iID); ASSERT_OPENGL();
}

void shader_program::validate()
{
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

void gfx::shader_program::setup() {
	glUseProgram(m_iID); ASSERT_OPENGL();
	get_uniform_location(m_qc, "uniform_trans", &m_iUniformMatTrans);
	get_uniform_location(m_qc, "uniform_view", &m_iUniformMatView);
	get_uniform_location(m_qc, "uniform_proj", &m_iUniformMatProj);

	m_iUniformTex1 = glGetUniformLocation(m_iID, "tex_tex1"); ASSERT_OPENGL();
	m_iUniformTex2 = glGetUniformLocation(m_iID, "tex_tex2"); ASSERT_OPENGL();
	m_iUniformTex3 = glGetUniformLocation(m_iID, "tex_tex3"); ASSERT_OPENGL();
	m_iUniformTex4 = glGetUniformLocation(m_iID, "tex_tex4"); ASSERT_OPENGL();
	m_iUniformTex5 = glGetUniformLocation(m_iID, "tex_tex5"); ASSERT_OPENGL();
	m_iUniformTime = glGetUniformLocation(m_iID, "game_time"); ASSERT_OPENGL();

	glUniform1i(m_iUniformTex1, 0); ASSERT_OPENGL();
	glUniform1i(m_iUniformTex2, 1); ASSERT_OPENGL();
	glUniform1i(m_iUniformTex3, 2); ASSERT_OPENGL();
	glUniform1i(m_iUniformTex4, 3); ASSERT_OPENGL();
	glUniform1i(m_iUniformTex5, 4); ASSERT_OPENGL();

	if (m_qc.count("diffuse_key"))
		m_mapTexKey.emplace(SHADERTEX_DIFFUSE, m_qc.at<std::string>("diffuse_key"));
	if (m_qc.count("normal_key"))
		m_mapTexKey.emplace(SHADERTEX_NORMAL, m_qc.at<std::string>("normal_key"));
	if (m_qc.count("specular_key"))
		m_mapTexKey.emplace(SHADERTEX_SPECULAR, m_qc.at<std::string>("specular_key"));
	if (m_qc.count("opacity_key"))
		m_mapTexKey.emplace(SHADERTEX_OPACITY, m_qc.at<std::string>("opacity_key"));

	if (m_qc.count("diffuse_default"))
		m_mapTexDefault.emplace(SHADERTEX_DIFFUSE, m_qc.at<std::string>("diffuse_default"));
	if (m_qc.count("normal_default"))
		m_mapTexDefault.emplace(SHADERTEX_NORMAL, m_qc.at<std::string>("normal_default"));
	if (m_qc.count("specular_default"))
		m_mapTexDefault.emplace(SHADERTEX_SPECULAR, m_qc.at<std::string>("specular_default"));
	if (m_qc.count("opacity_default"))
		m_mapTexDefault.emplace(SHADERTEX_OPACITY, m_qc.at<std::string>("opacity_default"));

	set_bool("bDebugDrawNormalsOnly", false);

	m_bLit = m_qc.count("lit") && m_qc.at<int>("lit") == 1;
	if (m_bLit) {
		std::vector<std::string> all_parameters = {
			"light_no_global",
			"lightl_pos_x", "lightl_pos_y","lightl_pos_z",
			"lightg_rot_x", "lightg_rot_y","lightg_rot_z",
			"lightl_color_r", "lightl_color_g","lightl_color_b","lightl_color_a",
			"lightg_color_r", "lightg_color_g","lightg_color_b","lightg_color_a",
		};
		std::vector<std::string> missing_parameters;

		bool bAllParametersSpecified = true;

		for (auto& param : all_parameters) {
			if (!m_qc.count(param.c_str())) {
				bAllParametersSpecified = false;
				missing_parameters.push_back(param);
			}
		}

		if (!bAllParametersSpecified) {
			m_bLit = false;
			PRINT_ERR("Shader " << m_szName << " is marked as 'lit' but one or more lighting parameters are missing: ");
			for (auto& param : missing_parameters) {
				PRINT_ERR(param);
			}
			PRINT_ERR("======= End of missing parameters, cut here =======");
		} else {
			// Local color position
			auto iszPosLX = m_qc.at<std::string>("lightl_pos_x");
			auto iszPosLY = m_qc.at<std::string>("lightl_pos_y");
			auto iszPosLZ = m_qc.at<std::string>("lightl_pos_z");

			// Global light rotation
			auto iszRotGX = m_qc.at<std::string>("lightg_rot_x");
			auto iszRotGY = m_qc.at<std::string>("lightg_rot_y");
			auto iszRotGZ = m_qc.at<std::string>("lightg_rot_z");

			// Local light color
			auto iszColorLR = m_qc.at<std::string>("lightl_color_r");
			auto iszColorLG = m_qc.at<std::string>("lightl_color_g");
			auto iszColorLB = m_qc.at<std::string>("lightl_color_b");
			auto iszColorLA = m_qc.at<std::string>("lightl_color_a");

			// Global light color
			auto iszColorGR = m_qc.at<std::string>("lightg_color_r");
			auto iszColorGG = m_qc.at<std::string>("lightg_color_g");
			auto iszColorGB = m_qc.at<std::string>("lightg_color_b");
			auto iszColorGA = m_qc.at<std::string>("lightg_color_a");

			auto iszDisabledL = m_qc.at<std::string>("lightl_disabled");
			auto iszDisabledG = m_qc.at<std::string>("lightg_disabled");

#define GET_LIGHT_UNILOC(type, tar, id) m_aiUniformLight##type .tar = glGetUniformLocation(m_iID, isz##id.c_str()); ASSERT_OPENGL(); if(m_aiUniformLight##type .tar == -1) PRINT_ERR("Failed getting uniform: " << isz##id);

			GET_LIGHT_UNILOC(Local, flPosX, PosLX);
			GET_LIGHT_UNILOC(Local, flPosY, PosLY);
			GET_LIGHT_UNILOC(Local, flPosZ, PosLZ);
			GET_LIGHT_UNILOC(Local, flColorR, ColorLR);
			GET_LIGHT_UNILOC(Local, flColorG, ColorLG);
			GET_LIGHT_UNILOC(Local, flColorB, ColorLB);
			GET_LIGHT_UNILOC(Local, flColorA, ColorLA);
			GET_LIGHT_UNILOC(Local, bLight, DisabledL);

			GET_LIGHT_UNILOC(Global, flPosX, RotGX);
			GET_LIGHT_UNILOC(Global, flPosY, RotGY);
			GET_LIGHT_UNILOC(Global, flPosZ, RotGZ);
			GET_LIGHT_UNILOC(Global, flColorR, ColorGR);
			GET_LIGHT_UNILOC(Global, flColorG, ColorGG);
			GET_LIGHT_UNILOC(Global, flColorB, ColorGB);
			GET_LIGHT_UNILOC(Global, flColorA, ColorGA);
			GET_LIGHT_UNILOC(Global, bLight, DisabledG);
		}
	}

	if (m_qc.count("depth_texture")) {
		get_uniform_location(m_qc, "depth_texture", &m_iUniformDepthTexture);
		glUniform1i(m_iUniformDepthTexture, 5); ASSERT_OPENGL();
	} else {
		m_iUniformDepthTexture = -1;
	}

	glUseProgram(0);
}

void gfx::shader_program::reload() {
	bool bModified = false;
	if (m_watchdog_vert.modified()) {
		bModified = true;
		glDetachShader(m_iID, m_pShaderVert->get_id());
		m_pShaderVert = std::make_shared<shader>(m_sPathVert.c_str(), SHADER_T_VERTEX);
		glAttachShader(m_iID, m_pShaderVert->get_id());
		ASSERT(m_pShaderVert);
	}
	if (m_watchdog_frag.modified()) {
		bModified = true;
		glDetachShader(m_iID, m_pShaderFrag->get_id());
		m_pShaderFrag = std::make_shared<shader>(m_sPathFrag.c_str(), SHADER_T_FRAGMENT);
		glAttachShader(m_iID, m_pShaderFrag->get_id());
		ASSERT(m_pShaderFrag);
	}

	if (bModified) {
		link();
		setup();
	}
}

void gfx::shader_program::attach_shader(gfx::shared_shader pShader)
{
	switch (pShader->get_type()) {
	case shader_type::SHADER_T_VERTEX:
		m_pShaderVert = pShader;
		break;
	case shader_type::SHADER_T_FRAGMENT:
		m_pShaderFrag = pShader;
		break;
	case shader_type::SHADER_T_GEOMETRY:
		m_pShaderGeom = pShader;
		break;
	}
	glAttachShader(m_iID, pShader->get_id());
}

void shader_program::print_err()
{
	char szMsg[512];
	glGetProgramInfoLog(m_iID, 512, NULL, szMsg);
	PRINT_ERR("Shader program link failed: " << szMsg);
}

void shader_program::set_mat_trans(void * pMat)
{
	glUseProgram(m_iID); ASSERT_OPENGL();
	glUniformMatrix4fv(m_iUniformMatTrans, 1, GL_FALSE, (const GLfloat*)pMat); ASSERT_OPENGL();
}

void shader_program::set_mat_view(void * pMat)
{
	glUseProgram(m_iID); ASSERT_OPENGL();
	glUniformMatrix4fv(m_iUniformMatView, 1, GL_FALSE, (const GLfloat*)pMat); ASSERT_OPENGL();
}

void shader_program::set_mat_proj(void * pMat)
{
	glUseProgram(m_iID); ASSERT_OPENGL();
	glUniformMatrix4fv(m_iUniformMatProj, 1, GL_FALSE, (const GLfloat*)pMat); ASSERT_OPENGL();
}

bool shader_program::load_material(material & mat)
{
	const mdlc::qc& qcp = mat.get_parser();

	for (size_t iTex = 0; iTex < SHADERTEX_MAX; iTex++) {
		auto iszKey = m_mapTexKey[(shader_tex_type)iTex];
		uint32_t iTexObj = 0;
		if (qcp.is_cmd(iszKey.c_str())) {
			iTexObj = gpGfx->load_texture(qcp.at<std::string>(iszKey.c_str()));
			ASSERT(iTexObj);
			if (!iTexObj)
				return false;
		}
		else {
			auto iszDefaultTex = m_mapTexDefault[(shader_tex_type)iTex];
			iTexObj = gpGfx->load_texture(iszDefaultTex);
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
		uint32_t iTex = mat.get_texture((mat_tex_index)i);
		glBindTexture(GL_TEXTURE_2D, iTex); ASSERT_OPENGL();
	}
}

void shader_program::set_vec3(const std::string & name, const math::vector3<float>& v)
{
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if(iLoc != -1)
		glUniform3fv(iLoc, 1, v.ptr()); ASSERT_OPENGL();
	
}

void shader_program::set_vec4(const std::string & name, const float * v)
{
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if (iLoc != -1)
		glUniform4fv(iLoc, 1, v); ASSERT_OPENGL();
}

void shader_program::set_mat4(const std::string & name, const void * m)
{
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if (iLoc != -1)
		glUniformMatrix4fv(iLoc, 1, GL_FALSE, (const GLfloat*)m); ASSERT_OPENGL();
}

void shader_program::set_bool(const std::string & name, bool v)
{
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if (iLoc != -1)
		glUniform1i(iLoc, (v ? 1 : 0)); ASSERT_OPENGL();
}

void shader_program::set_local_light(const shader_light & l)
{

	if (l.iType == shader_light_type::SLT_GLOBAL) {
		PRINT_DBG("shader_program::set_local_light: shader_light passed to me describes a global light, redirected!");
		set_global_light(l);
		return;
	}

	glUseProgram(m_iID);
	if (l.iType == shader_light_type::SLT_DISABLED) {
		glUniform1i(m_aiUniformLightLocal.bLight, 1);
		return;
	}
	glUniform1i(m_aiUniformLightLocal.bLight, 0);
	glUniform1f(m_aiUniformLightLocal.flPosX, l.pos[0]);
	glUniform1f(m_aiUniformLightLocal.flPosY, l.pos[1]);
	glUniform1f(m_aiUniformLightLocal.flPosZ, l.pos[2]);
	glUniform1f(m_aiUniformLightLocal.flColorR, l.color.r);
	glUniform1f(m_aiUniformLightLocal.flColorG, l.color.g);
	glUniform1f(m_aiUniformLightLocal.flColorB, l.color.b);
	glUniform1f(m_aiUniformLightLocal.flColorA, l.color.a);
}

void shader_program::set_global_light(const shader_light & l)
{

	if (l.iType == shader_light_type::SLT_POINT) {
		PRINT_DBG("shader_program::set_global_light: shader_light passed to me describes a local light, redirected!");
		set_local_light(l);
		return;
	}

	glUseProgram(m_iID);
	if (l.iType == shader_light_type::SLT_DISABLED) {
		glUniform1i(m_aiUniformLightGlobal.bLight, 1);
		return;
	}
	glUniform1i(m_aiUniformLightGlobal.bLight, 0);
	glUniform1f(m_aiUniformLightGlobal.flPosX, l.pos[0]);
	glUniform1f(m_aiUniformLightGlobal.flPosY, l.pos[1]);
	glUniform1f(m_aiUniformLightGlobal.flPosZ, l.pos[2]);
	glUniform1f(m_aiUniformLightGlobal.flColorR, l.color.r);
	glUniform1f(m_aiUniformLightGlobal.flColorG, l.color.g);
	glUniform1f(m_aiUniformLightGlobal.flColorB, l.color.b);
	glUniform1f(m_aiUniformLightGlobal.flColorA, l.color.a);
}

void gfx::shader_program::set_float(const std::string & name, float v)
{
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if (iLoc != -1)
		glUniform1f(iLoc, (GLfloat)v); ASSERT_OPENGL();
}

void gfx::shader_program::set_int(const std::string & name, int v) {
	auto iLoc = glGetUniformLocation(m_iID, name.c_str()); ASSERT_OPENGL();
	if (iLoc != -1)
		glUniform1i(iLoc, (GLint)v); ASSERT_OPENGL();
}

int shader_program::get_uniform_location(const mdlc::qc& qcp, const std::string& name, int* pLocation)
{
	const char* pszCmd = name.c_str();
	if (qcp.is_cmd(pszCmd))
	{
		std::string iszUniform = qcp.at<std::string>(pszCmd).c_str();
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
