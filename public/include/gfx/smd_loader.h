#pragma once
// Studiomdl Data parser
// 
// https://developer.valvesoftware.com/wiki/.smd

#include <string>
#include <vector>
#include <fstream>
#include <map>

#include <gfx/model.h>

namespace gfx {
	class smd_loader {
	public:
		smd_loader() {};
		smd_loader(const char* szFilename);
		smd_loader(std::string& iszFilename);

		gfx::model get_model() const {
			return m_outmodel;
		}

		bool fail() const {
			return m_bFail;
		}

	protected:
		void parse();
		void parse_line();

		std::vector<std::string> tokenize(const std::string& line) const;

	private:
		bool m_bFail = false;
		int m_iLine;
		int m_iSkeletonTime;
		int m_iState;
		std::ifstream m_file;

		model m_outmodel;
		gfx::triangle m_triangle;
		std::vector<gfx::keyframe> keyframes;
	};
}