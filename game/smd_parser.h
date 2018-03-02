#pragma once
// Studiomdl Data parser
// 
// https://developer.valvesoftware.com/wiki/.smd

#include <string>
#include <vector>
#include <fstream>
#include <map>

#include <model.h>

namespace mdlc {
	class smd_parser {
	public:
		smd_parser() {};
		smd_parser(const char* szFilename);
		smd_parser(std::string& iszFilename);

		model get_model() const {
			return m_outmodel;
		}
	protected:
		void parse();
		void parse_line();

		std::vector<std::string> tokenize(const std::string& line) const;

	private:
		int m_iLine;
		int m_iSkeletonTime;
		int m_iState;
		std::ifstream m_file;

		model m_outmodel;
		model_triangle m_triangle;
	};
}