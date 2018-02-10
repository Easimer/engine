#pragma once
// Studiomdl Data parser
// 
// https://developer.valvesoftware.com/wiki/.smd

#include <string>
#include <vector>
#include <fstream>
#include <map>

namespace mdlc {

	struct smd_vertex {
		float px, py, pz;
		float nx, ny, nz;
		float u, v;
	};

	struct smd_triangle {
		size_t material;
		// these seem to be clockwise
		smd_vertex vertices[3];
	};


	struct smd_bone {
		int iId;
		int iParent;
		std::string iszName;
	};

	struct smd_keyframe {
		int iTime;
		int iBone;
		float px, py, pz;
		float rx, ry, rz;
	};

	class smd_parser {
	public:
		smd_parser(const char* szFilename);
		smd_parser(std::string& iszFilename);
		

	protected:
		void parse();
		void parse_line();

	private:
		int m_iLine;
		int m_iSkeletonTime;
		int m_iState;
		std::ifstream m_file;

		// <material, material table index>
		std::map<std::string, size_t> m_materials;
	};
}