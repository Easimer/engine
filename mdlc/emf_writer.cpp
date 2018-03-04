#include "stdafx.h"
#include "emf_writer.h"

void emf_writer::set_outfile(const std::string & outfile)
{
	m_iszOutfile = outfile;
}

void emf_writer::set_mesh(const std::vector<model_triangle>& triangles)
{
	m_mesh = triangles;
}

void emf_writer::set_collmesh(const std::vector<model_triangle>& triangles)
{
	m_collmesh = triangles;
}

void emf_writer::add_animation(const std::string& name, const std::vector<model_keyframe>& animation)
{
	m_animations.emplace(name, animation);
}

void emf_writer::add_bones(const std::vector<model_bone>& bones)
{
	for (auto& bone : bones) {
		m_bones.push_back(bone);
	}
}
