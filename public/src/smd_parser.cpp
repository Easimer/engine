#include "stdafx.h"
#include "smd_parser.h"
#include <sstream>
#include <algorithm>
#include <enl/strstrip.h>
#include <parseutils.h>
#include <enl/benchmark.h>
#include <enl/assert.h>

#define SMDP_STATE_START			0x00
#define SMDP_STATE_GLOBAL			0x10
#define SMDP_STATE_NODES			0x20
#define SMDP_STATE_TRIANGLE_MAT		0x40
#define SMDP_STATE_TRIANGLE_VTX1	0x41
#define SMDP_STATE_TRIANGLE_VTX2	0x42
#define SMDP_STATE_TRIANGLE_VTX3	0x43
#define SMDP_STATE_SKELETON			0x80

#define SMDP_STATE_TRIANGLE			0x40

#define SMDP_CHECK_END()	if(aiTokens[0] == "end") { m_iState = SMDP_STATE_GLOBAL; return; }

mdlc::smd_parser::smd_parser(const char * szFilename)
{
	m_file = std::ifstream(szFilename);
	if (!m_file) {
		PRINT_ERR("smd_parser: cannot open " << szFilename);
		//throw std::runtime_error("smd_parser: cannot open " + std::string(szFilename));
		m_bFail = true;
	}
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
		PRINT_ERR("smd_parser: cannot open " << iszFilename);
		//throw std::runtime_error("smd_parser: cannot open " + iszFilename);
		m_bFail = true;
	}
	parse();
}

void mdlc::smd_parser::parse()
{
	m_iLine = 0;
	m_iState = SMDP_STATE_START;

	benchmark bm;
	bm.start();
	while (m_file && !m_file.eof())
	{
		m_iLine++;
		parse_line();
	}
	bm.end();
	PRINT_DBG("Model loaded under " << bm.elapsed() << " secs");
}

void mdlc::smd_parser::parse_line()
{
	std::string line;
	std::getline(m_file, line);

	//std::cout << '\"' << szStrippedLine << '\"' << std::endl;
	//std::cout << m_iLine << std::endl;

	//std::getline(ss, token, ' ');
	auto aiTokens = tokenize(line);
	if (aiTokens.size() == 0)
		return;

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
	// Animation
	model_keyframe new_kf;
	model_keyframe cur_kf;
	model_bone_state bs;

	std::string nulbuf;

	switch (m_iState)
	{
	case SMDP_STATE_START:
		if (aiTokens[0] != "version")
		{
			PRINT_ERR("expected \"version\" on line " << m_iLine);
			return;
		}
		if (std::stoi(aiTokens[1]) != 1)
		{
			PRINT_ERR("Unknown SMD version " << aiTokens[1] << ", attempting parse anyway");
		}
		m_iState = SMDP_STATE_GLOBAL;
		break;

	case SMDP_STATE_GLOBAL:
		if (aiTokens.size() == 0 || aiTokens[0] == "")
			break;
		if (aiTokens[0] == "nodes")
			m_iState = SMDP_STATE_NODES;
		else if (aiTokens[0] == "skeleton")
			m_iState = SMDP_STATE_SKELETON;
		else if (aiTokens[0] == "triangles")
			m_iState = SMDP_STATE_TRIANGLE;
		else if (aiTokens[0] == "end")
			m_iState = SMDP_STATE_GLOBAL;
		else
			PRINT_DBG("smd_parser expected nodes, skeleton, triangles or end on line " << m_iLine << ", got: " << aiTokens[0]);
		break;

	case SMDP_STATE_NODES:
		SMDP_CHECK_END();
		bone_id = aiTokens[0];
		bone_name = aiTokens[1];
		bone_parent_id = aiTokens[2];

		bone_name = bone_name.substr(1, bone_name.size() - 2);

		//PRINT("Bone (#" << bone_id << ") \"" << bone_name << "\", parent: #" << bone_parent_id);
		bone.iID = std::stoi(bone_id);
		bone.iParentID = std::stoi(bone_parent_id);
		bone_name.copy(bone.szName, SMD_MAX_BONE_NAME_SIZ);

		m_outmodel.bones.push_back(bone);
		break;
	case SMDP_STATE_SKELETON:
		SMDP_CHECK_END();
		if (aiTokens[0] == "time") {
			new_kf.iFrame = std::stoi(aiTokens[1]);
			keyframes.push_back(new_kf);
			break;
		}

		ASSERT(keyframes.size());

		cur_kf = keyframes.back();

		bs.px = std::stof(aiTokens[1]);
		bs.py = std::stof(aiTokens[2]);
		bs.pz = std::stof(aiTokens[3]);

		bs.rx = std::stof(aiTokens[4]);
		bs.ry = std::stof(aiTokens[5]);
		bs.rz = std::stof(aiTokens[6]);

		cur_kf.bones.push_back({ std::stoi(aiTokens[0]), bs });
		
		break;
	case SMDP_STATE_TRIANGLE_MAT:
		SMDP_CHECK_END();

		m_triangle.iModelMaterial = -1;

		// check if material is already used
		for (auto& mat : m_outmodel.materials)
		{
			if (strcmp(mat.szName, aiTokens[0].c_str()) == 0)
			{
				m_triangle.iModelMaterial = mat.iModelMaterial;
				break;
			}
		}

		if (m_triangle.iModelMaterial == -1)
		{
			m_outmodel.iLastMatID++;
			strncpy(material.szName, aiTokens[0].c_str(), SMD_MAX_MATERIAL_PATH_SIZ);
			material.iModelMaterial = m_outmodel.iLastMatID;
			m_outmodel.materials.push_back(material);
			m_triangle.iModelMaterial = m_outmodel.iLastMatID;
		}

		m_iState = SMDP_STATE_TRIANGLE_VTX1;
		break;

#define SMDP_VTX_STOF(var) vertex.var = std::stof(var)
#define SMDP_TOK(var, i) var = aiTokens[i]
#define SMDP_VTX()															\
			SMDP_TOK(iBoneID, 0);												\
			SMDP_TOK(px, 1); SMDP_TOK(py, 2); SMDP_TOK(pz, 3);					\
			SMDP_TOK(nx, 4); SMDP_TOK(ny, 5); SMDP_TOK(nz, 6);					\
			SMDP_TOK(u, 7); SMDP_TOK(v, 8);									\
			SMDP_VTX_STOF(iBoneID);											\
			SMDP_VTX_STOF(px); SMDP_VTX_STOF(py); SMDP_VTX_STOF(pz);		\
			SMDP_VTX_STOF(nx); SMDP_VTX_STOF(ny); SMDP_VTX_STOF(nz);		\
			SMDP_VTX_STOF(u); SMDP_VTX_STOF(v);						\
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
}

std::vector<std::string> mdlc::smd_parser::tokenize(const std::string & line) const
{
	std::vector<std::string> ret;
	size_t iStart = 0;
	bool bEnded = false;

	for (size_t i = 0; i < line.size(); i++) {

		if (line[i] == ' ') {
			if (!bEnded) {
				bEnded = true;
				ret.push_back(line.substr(iStart, i - iStart));
			}
		}
		else {
			if (bEnded) {
				iStart = i;
				bEnded = false;
			}
		}
		/*
		if (line[i] == ' ' && !bEnded) {
			
		}
		if (line[i] == ' ' && bEnded) {
			continue;
		}
		if (line[i] != ' ' && bEnded) {
			iStart = i;
			bEnded = false;
		}
		*/
	}

	if (!bEnded) {
		ret.push_back(line.substr(iStart, line.size() - iStart));
	}

	return ret;
}
