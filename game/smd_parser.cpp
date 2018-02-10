#include "stdafx.h"
#include <parseutils.h>
#include "smd_parser.h"
#include <sstream>
#include <algorithm>
#include <enl/strstrip.h>

#define SMDP_STATE_START			0x00
#define SMDP_STATE_GLOBAL			0x10
#define SMDP_STATE_NODES			0x20
#define SMDP_STATE_TRIANGLE_MAT		0x40
#define SMDP_STATE_TRIANGLE_VTX1	0x41
#define SMDP_STATE_TRIANGLE_VTX2	0x42
#define SMDP_STATE_TRIANGLE_VTX3	0x43
#define SMDP_STATE_SKELETON			0x80

#define SMDP_STATE_TRIANGLE			0x40

#define SMDP_CHECK_END()	if(token == "end") { m_iState = SMDP_STATE_GLOBAL; return; }

mdlc::smd_parser::smd_parser(const char * szFilename)
{
	m_file = std::ifstream(szFilename);
	if (!m_file)
		throw std::runtime_error("smd_parser: cannot open " + std::string(szFilename));
	parse();
}

mdlc::smd_parser::smd_parser(std::string & iszFilename)
{
	iszFilename.erase(std::remove_if(iszFilename.begin(), iszFilename.end(), [](const char c) {
		return mdlc::is_whitespace(c);
	}), iszFilename.end());
	m_file = std::ifstream(iszFilename);
	if (!m_file)
	{
		PRINT_ERR("smd_parser: cannot open " + iszFilename);
		throw std::runtime_error("smd_parser: cannot open " + iszFilename);
	}
	else
		PRINT("Opened " << iszFilename);
	parse();
}

void mdlc::smd_parser::parse()
{
	m_iLine = 0;
	m_iState = SMDP_STATE_START;
	while (m_file && !m_file.eof())
	{
		m_iLine++;
		parse_line();
	}
}

#define SMDP_TOKEN(dst) std::getline(ss, dst, ' ');

void mdlc::smd_parser::parse_line()
{
	std::string line;
	std::string token;
	std::getline(m_file, line);

	char szLine[256] = { 0 };
	char* szStrippedLine;
	line.copy(szLine, 256);
	szStrippedLine = strnlstrip(szLine, 256);

	std::stringstream ss(szStrippedLine);

	//std::cout << '\"' << szStrippedLine << '\"' << std::endl;
	//std::cout << m_iLine << std::endl;

	std::getline(ss, token, ' ');

	//std::cout << "State is " << m_iState << std::endl;

	// Bone
	model_bone bone;
	std::string bone_id;
	std::string bone_name;
	std::string bone_parent_id;
	// Triangle
	model_material material;
	// Vertex
	model_triangle_vertex vertex;
	std::string iBoneID;
	std::string px, py, pz;
	std::string u, v;
	std::string nx, ny, nz;

	std::string nulbuf;

	switch (m_iState)
	{
	case SMDP_STATE_START:
		if (token != "version")
		{
			PRINT_ERR("expected \"version\" on line " << m_iLine);
			return;
		}
		SMDP_TOKEN(token);
		//PRINT_DBG("version " << token << " studiomdl file");
		if (std::stoi(token) != 1)
		{
			PRINT_ERR("Unknown SMD version " << token << ", attempting parse anyway");
		}
		m_iState = SMDP_STATE_GLOBAL;
		break;

	case SMDP_STATE_GLOBAL:
		if (token == "nodes")
			m_iState = SMDP_STATE_NODES;
		else if (token == "skeleton")
			m_iState = SMDP_STATE_SKELETON;
		else if (token == "triangles")
			m_iState = SMDP_STATE_TRIANGLE;
		else if (token == "end")
			m_iState = SMDP_STATE_GLOBAL;
		break;

	case SMDP_STATE_NODES:
		SMDP_CHECK_END();
		bone_id = token;
		SMDP_TOKEN(bone_name);
		SMDP_TOKEN(bone_parent_id);

		bone_name = bone_name.substr(1, bone_name.size() - 2);

		//PRINT("Bone (#" << bone_id << ") \"" << bone_name << "\", parent: #" << bone_parent_id);
		bone.iID = std::stoi(bone_id);
		bone.iParentID = std::stoi(bone_parent_id);
		bone_name.copy(bone.szName, SMD_MAX_BONE_NAME_SIZ);

		m_outmodel.bones.push_back(bone);
		break;
	case SMDP_STATE_SKELETON:
		SMDP_CHECK_END();
		break;
	case SMDP_STATE_TRIANGLE_MAT:
		SMDP_CHECK_END();

		m_triangle.iModelMaterial = -1;

		// check if material is already used
		for (auto& mat : m_outmodel.materials)
		{
			if (strcmp(mat.szName, szStrippedLine) == 0)
			{
				m_triangle.iModelMaterial = mat.iModelMaterial;
				break;
			}
		}

		if (m_triangle.iModelMaterial == -1)
		{
			m_outmodel.iLastMatID++;
			strncpy(material.szName, szStrippedLine, SMD_MAX_MATERIAL_PATH_SIZ);
			//PRINT_DBG("Added material " << m_outmodel.iLastMatID << ':' << szStrippedLine);
			material.iModelMaterial = m_outmodel.iLastMatID;
			m_outmodel.materials.push_back(material);
			m_triangle.iModelMaterial = m_outmodel.iLastMatID;
		}

		m_iState = SMDP_STATE_TRIANGLE_VTX1;
		break;

#define SMDP_VTX_STOF(var) vertex.var = std::stof(var)
#define SMDP_VTX()															\
			iBoneID = token;												\
			SMDP_TOKEN(px); SMDP_TOKEN(py); SMDP_TOKEN(pz);					\
			SMDP_TOKEN(nx); SMDP_TOKEN(ny); SMDP_TOKEN(nz);					\
			SMDP_TOKEN(u); SMDP_TOKEN(v);									\
			SMDP_TOKEN(nulbuf); SMDP_TOKEN(nulbuf); SMDP_TOKEN(nulbuf);		\
			SMDP_VTX_STOF(iBoneID);											\
			SMDP_VTX_STOF(px); SMDP_VTX_STOF(py); SMDP_VTX_STOF(pz);		\
			SMDP_VTX_STOF(nx); SMDP_VTX_STOF(ny); SMDP_VTX_STOF(nz);		\
			SMDP_VTX_STOF(u); SMDP_VTX_STOF(v);								\
			m_triangle.vertices.push_back(vertex);

	case SMDP_STATE_TRIANGLE_VTX1:
		SMDP_CHECK_END();
		SMDP_VTX();
		m_iState = SMDP_STATE_TRIANGLE_VTX2;
		break;
	case SMDP_STATE_TRIANGLE_VTX2:
		SMDP_CHECK_END();
		SMDP_VTX();
		m_iState = SMDP_STATE_TRIANGLE_VTX3;
		break;
	case SMDP_STATE_TRIANGLE_VTX3:
		SMDP_CHECK_END();
		SMDP_VTX();
		m_iState = SMDP_STATE_TRIANGLE_MAT;
		m_outmodel.triangles.push_back(m_triangle);
		m_triangle.iModelMaterial = -1;
		m_triangle.vertices.clear();
		break;
	}
	//std::cin >> line;
}
