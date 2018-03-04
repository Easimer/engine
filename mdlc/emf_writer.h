#pragma once

#include <vector>
#include <map>
#include <string>
#include "model.h"

class emf_writer {
public:
	void set_outfile(const std::string& outfile);

	void set_mesh(const std::vector<model_triangle>& triangles);
	void set_collmesh(const std::vector<model_triangle>& triangles);
	void add_animation(const std::string& name, const std::vector<model_keyframe>& animation);
	void add_bones(const std::vector<model_bone>& bones);

private:
	std::string m_iszOutfile;

	std::vector<model_triangle> m_mesh;
	std::vector<model_triangle> m_collmesh;
	std::map<std::string, std::vector<model_keyframe>> m_animations;
	std::vector<model_bone> m_bones;
};
