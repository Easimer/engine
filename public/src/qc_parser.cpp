#include "stdafx.h"
#include "qc_parser.h"
#include <sstream>
#include <algorithm>
#include <parseutils.h>

mdlc::qc_parser::qc_parser(const char * szFilename)
{
	m_file = std::ifstream(szFilename);
	if (m_file.fail())
	{
		PRINT_DBG("Can't open file " << szFilename << ", reason: " << strerror(errno));
		return;
	}
	parse();
}

mdlc::qc_parser::qc_parser(std::string & iszFilename)
{
	m_file = std::ifstream(iszFilename);
	if (m_file.fail())
	{
		PRINT_DBG("Can't open file " << iszFilename << ", reason: " << strerror(errno));
		return;
	}
	parse();
}

void mdlc::qc_parser::operator=(const qc_parser & other)
{
	// Copy commands only
	m_commands = other.m_commands;
}

bool mdlc::qc_parser::is_cmd(const char* szCmd) const
{
	return m_commands.count(std::string(szCmd)) == 1;
}

int mdlc::qc_parser::get_int(const char* szCmd) const
{
	return std::stoi(m_commands.at(std::string(szCmd)));
}

double mdlc::qc_parser::get_float(const char* szCmd) const
{
	return std::stod(m_commands.at(std::string(szCmd)));
}

std::string mdlc::qc_parser::get_string(const char* szCmd) const
{
	return m_commands.at(std::string(szCmd));
}

std::vector<std::string> mdlc::qc_parser::get_commands() const
{
	std::vector<std::string> ret;
	for (auto& cmd : m_commands) {
		ret.push_back(cmd.first);
	}

	return ret;
}

void mdlc::qc_parser::parse()
{
	m_iLine = 0;
	while (m_file && !m_file.eof())
	{
		m_iLine++;
		parse_line();
	}
}

void mdlc::qc_parser::parse_line()
{
	std::string line;
	std::getline(m_file, line);
	// is argument in quotes
	bool bQuotes = false;
	std::stringstream cmd, arg;
	size_t i;
	char c;

	if (line.size() < 2) // i.e.: empty line or "$a"
	{
		//PRINT_ERR("ERR1: line" << m_iLine << " too short");
		return;
	}

	i = 0;

	while (line[i] == ' ')
		i++;

	if (line.size() < i + 3) {
		return;
	}
	if (line[i] == '/' && line[i + 1] == '/') { // Comment
		return;
	}

	if (line[i] != '$')
	{
		PRINT_ERR("ERR2: expected '$', instead of '" << line[i] << "' on line " << m_iLine);
		return;
	}

	if (!mdlc::is_letter(line[++i]))
	{
		PRINT_ERR("ERR3: expected letter, instead of '" << line[i] << "' on line " << m_iLine);
		return;
	}

	while (!is_whitespace(line[i]))
	{
		c = line[i];
		if (mdlc::is_letter(c) || c == '_')
		{
			cmd << c;
			i++;
		}
		else
		{
			PRINT_ERR("ERR4: expected letter, instead of '" << c << "' on line " << m_iLine);
			break;
		}
	}

	i++;

	while (c == ' ' || c == '\t') {
		c = line[++i];
	}

	if (line[i] == '\n')
		goto over;

	while (line[i] != '\n')
	{
		c = line[i];
		if (c == '"')
		{
			if (!bQuotes)
			{
				bQuotes = true;
				i++;
				continue;
			}
			if (bQuotes)
			{
				break;
			}
		}
		if (mdlc::is_path(c)) // includes alphanumeric characters plus some more
		{
			arg << c;
			i++;
			continue;
		}
		if (c == ' ' && !bQuotes)
		{
			PRINT_ERR("ERR5: space(s) in a string without surrounding quotation marks on line " << m_iLine);
			break;
		}
		if (c == '\t')
		{
			i++;
			continue;
		}
	}

over:
	m_commands.emplace(cmd.str(), arg.str());
}
