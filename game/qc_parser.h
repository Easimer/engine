#pragma once
// QC parser
// Simplified version of the Valve QC format
// https://developer.valvesoftware.com/wiki/QC
// No options, no $include

#include <string>
#include <vector>
#include <fstream>

namespace mdlc {

	struct qc_command {
		std::string m_iszCmd;
		std::string m_iszArg;
	};

	class qc_parser {
	public:
		qc_parser(const char* szFilename);
		qc_parser(std::string& iszFilename);

		bool is_cmd(const char* cmd);

		int get_int(const char* cmd);
		double get_float(const char* cmd);
		std::string get_string(const char* cmd);

	protected:
		void parse();
		void parse_line();

	private:
		int m_iLine;
		std::ifstream m_file;

		std::vector<qc_command> m_commands;
	};
}