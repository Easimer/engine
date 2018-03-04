#include "stdafx.h"
#include "emf_writer.h"
#include <emf.h>
#include <arch.h>
#include <fstream>

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

void emf_writer::write() const
{
	// construct filenames
	std::string iszPathManifest = m_iszOutfile;
	std::string iszPathMesh = m_iszOutfile + ".vtx";
	std::string iszPathColl = m_iszOutfile + ".coll";
	std::string iszPathSkel = m_iszOutfile + ".skel";
	std::string iszPathAnim = m_iszOutfile + ".anim";

	emf_hdr hdr;
	hdr.iID = htonll(EMF_MAGIC);
	

	// Write manifest
	PRINT_DBG("Writing manifest");
	
	hdr.iType = htonll(EMF_T_MANIFEST);

	std::ofstream hManifest(iszPathManifest, std::ios::out | std::ios::binary);
	if (hManifest.fail()) {
		PRINT_ERR("Cannot open manifest file for writing: " << iszPathManifest);
		return;
	}

	hManifest.write((const char*)&hdr, sizeof(hdr));

	// Allocate mem for manifest struct and the animation names
	emf_manifest* pManifest = (emf_manifest*)malloc(sizeof(emf_manifest) + m_animations.size() * 64 * sizeof(char));

	strncpy(pManifest->szMaterial, m_iszMaterial.c_str(), 64);
	pManifest->nAnimations = m_animations.size();
	pManifest->nFramerate = m_nFramerate;
	memset(pManifest->szMaterial, 0, 256);
	strncpy(pManifest->szMaterial, m_iszMaterial.c_str(), 256);

	/// Write animation names
	char* pAnimNames = (char*)(pManifest + 1);
	memset(pAnimNames, 0, 64 * sizeof(char) * m_animations.size());
	for (auto& kv : m_animations) {
		pAnimNames[0] = '\0';
		pAnimNames += 64;
		strncpy(pAnimNames, kv.first.c_str(), 64);
		PRINT_DBG("Adding animation to manifest " << kv.first);
	}

	hManifest.write((const char*)pManifest, sizeof(emf_manifest) + m_animations.size() * 64 * sizeof(char));
	free(pManifest);

	PRINT_DBG("Written " << hManifest.cur << " bytes");

	hManifest.close();
}
