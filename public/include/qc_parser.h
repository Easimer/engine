#pragma once
// QC parser
// Implementation of version 2
// Simplified version of the Valve QC format
// https://developer.valvesoftware.com/wiki/QC
// No options, no $include

#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace mdlc {

	struct qc_command {
		std::string m_iszCmd;
		std::string m_iszArg;
	};

	class qc_parser {
	public:
		qc_parser() {}
		qc_parser(const char* szFilename);
		qc_parser(const std::string& iszFilename);

		void operator=(const qc_parser& other);

		bool is_cmd(const char* cmd) const;

		int get_int(const char* cmd) const;
		double get_float(const char* cmd) const;
		std::string get_string(const char* cmd) const;

		std::vector<std::string> get_commands() const;

	protected:
		void parse();
		void parse_line();

	private:
		int m_iLine;
		std::ifstream m_file;

		//std::vector<qc_command> m_commands;
		std::map<std::string, std::string> m_commands;
	};
}