#pragma once

#include <vector>
#include <map>
#include <string>
#include <gfx/model.h>

class emf_writer {
public:
	void set_outfile(const std::string& outfile);

	void set_mesh(const std::vector<gfx::triangle>& triangles);
	void set_collmesh(const std::vector<gfx::triangle>& triangles);
	void add_animation(const std::string& name, const std::vector<gfx::keyframe>& animation);
	void add_bones(const std::vector<gfx::bone>& bones);
	void set_framerate(unsigned nFramerate) { m_nFramerate = nFramerate; }
	void set_material(const std::string& iszMaterial) { m_iszMaterial = iszMaterial; }

	const std::string& get_outfile() const { return m_iszOutfile; }

	void write() const;

private:
	std::string m_iszOutfile;
	std::string m_iszMaterial;
	unsigned m_nFramerate;

	std::vector<gfx::triangle> m_mesh;
	std::vector<gfx::triangle> m_collmesh;
	std::map<std::string, std::vector<gfx::keyframe>> m_animations;
	std::vector<gfx::bone> m_bones;
};
