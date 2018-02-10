#include "stdafx.h"
#include <parseutils.h>
#include "smd_parser.h"
#include <sstream>
#include <algorithm>

#define SMDP_STATE_START			0x00
#define SMDP_STATE_HEADER			0x10
#define SMDP_STATE_NODES			0x20
#define SMDP_STATE_TRIANGLE_MAT		0x40
#define SMDP_STATE_TRIANGLE_VTX1	0x41
#define SMDP_STATE_TRIANGLE_VTX2	0x42
#define SMDP_STATE_TRIANGLE_VTX3	0x43
#define SMDP_STATE_SKELETON			0x80

#define SMDP_STATE_TRIANGLE			0x40

mdlc::smd_parser::smd_parser(const char * szFilename)
{
	m_file = std::ifstream(szFilename);
	if (!m_file)
		throw std::runtime_error("smd_parser: cannot open " + std::string(szFilename));
	parse();
}

mdlc::smd_parser::smd_parser(std::string & iszFilename)
{
	iszFilename.erase(std::remove_if(iszFilename.begin(), iszFilename.end(), [] (const char c){
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

void mdlc::smd_parser::parse_line()
{
	std::string line;
	std::string section;
	std::getline(m_file, line);
	std::stringstream ss(line);

	std::cout << line << std::endl;
	std::cout << m_iLine << std::endl;

	std::getline(ss, section);

	std::cout << section << std::endl;
}
