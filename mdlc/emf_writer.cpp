#include "stdafx.h"
#include "emf_writer.h"
#include <emf.h>
#include <arch.h>
#include <fstream>

#include <schemas/model_generated.h>


void emf_writer::set_outfile(const std::string & outfile)
{
	m_iszOutfile = outfile;
}

void emf_writer::set_mesh(const std::vector<gfx::triangle>& triangles)
{
	m_mesh = triangles;
}

void emf_writer::set_collmesh(const std::vector<gfx::triangle>& triangles)
{
	m_collmesh = triangles;
}

void emf_writer::add_animation(const std::string& name, const std::vector<gfx::keyframe>& animation)
{
	m_animations.emplace(name, animation);
}

void emf_writer::add_bones(const std::vector<gfx::bone>& bones)
{
	for (auto& bone : bones) {
		m_bones.push_back(bone);
	}
}

void emf_writer::write() const
{	
	flatbuffers::FlatBufferBuilder fbb;

	// Serialize triangles
	std::vector<flatbuffers::Offset<Schemas::Model::Triangle>> triangles;

	for (auto& triangle : m_mesh) {
		std::vector<flatbuffers::Offset<Schemas::Model::Vertex>> vertices;
		// Serialize vertices
		for (auto& vertex : triangle) {
			Schemas::Model::VertexBuilder fbb_vertex(fbb);
			auto pos= Schemas::Vector3(vertex.pos[0], vertex.pos[1], vertex.pos[2]);
			auto norm = Schemas::Vector3(vertex.normal[0], vertex.normal[1], vertex.normal[2]);
			fbb_vertex.add_pos(&pos);
			fbb_vertex.add_normal(&norm);
			fbb_vertex.add_u(vertex.u);
			fbb_vertex.add_v(vertex.v);
			fbb_vertex.add_bone(vertex.iBoneID);
			vertices.push_back(fbb_vertex.Finish());
		}
		auto fb_vertices = fbb.CreateVector(vertices);

		Schemas::Model::TriangleBuilder fbb_triangle(fbb);
		fbb_triangle.add_vertices(fb_vertices);

		triangles.push_back(fbb_triangle.Finish());
	}

	// Serialize skeleton
	std::vector<flatbuffers::Offset<Schemas::Model::Bone>> bones;

	for (auto& bone : m_bones) {
		auto fb_bonename = fbb.CreateString(bone.szName);
		Schemas::Model::BoneBuilder fbb_bone(fbb);

		fbb_bone.add_id(bone.iID);
		fbb_bone.add_name(fb_bonename);
		fbb_bone.add_parent(bone.iParentID);
		bones.push_back(fbb_bone.Finish());
	}

	// Serialize animations
	std::vector<flatbuffers::Offset<Schemas::Model::Animation>> animations;

	for (auto& kv_anim : m_animations) {
		std::vector<flatbuffers::Offset<Schemas::Model::Keyframe>> keyframes;
		for (auto& keyframe : kv_anim.second) {
			// Serialize bone state

			std::vector<flatbuffers::Offset<Schemas::Model::BoneState>> bonestates;

			for (auto& state : keyframe.bones) {
				Schemas::Vector3 pos(state.second.pos[0], state.second.pos[1], state.second.pos[2]);
				Schemas::Vector3 rot(state.second.rot[0], state.second.rot[1], state.second.rot[2]);
				Schemas::Model::BoneStateBuilder fbb_bonestate(fbb);
				fbb_bonestate.add_bone(state.first);
				fbb_bonestate.add_pos(&pos);
				fbb_bonestate.add_rot(&rot);
				bonestates.push_back(fbb_bonestate.Finish());
			}
			
			auto fb_bonestates = fbb.CreateVector(bonestates);

			Schemas::Model::KeyframeBuilder fbb_keyframe(fbb);
			fbb_keyframe.add_time(keyframe.iFrame);
			fbb_keyframe.add_bonestates(fb_bonestates);
			keyframes.push_back(fbb_keyframe.Finish());
		}
		
		auto fb_keyframes = fbb.CreateVector(keyframes);
		auto fb_name = fbb.CreateString(kv_anim.first);

		Schemas::Model::AnimationBuilder fb_animation(fbb);
		fb_animation.add_name(fb_name);
		fb_animation.add_keyframes(fb_keyframes);
		animations.push_back(fb_animation.Finish());
	}

	// Create offset vectors

	auto fb_triangles = fbb.CreateVector(triangles);
	auto fb_bones = fbb.CreateVector(bones);
	auto fb_material = fbb.CreateString(m_iszMaterial);
	auto fb_animations = fbb.CreateVector(animations);

	// Serialize model

	Schemas::Model::ModelBuilder fbb_model(fbb);

	fbb_model.add_material(fb_material);
	fbb_model.add_framerate(m_nFramerate);
	fbb_model.add_triangles(fb_triangles);
	fbb_model.add_animations(fb_animations);
	fbb_model.add_skeleton(fb_bones);

	Schemas::Model::FinishModelBuffer(fbb, fbb_model.Finish());
	
	// Write model to file

	/*std::ofstream file(m_iszOutfile, std::ios::binary);
	if (!file) {
		PRINT_ERR("Cannot open outfile!");
		return;
	}

	file.write((char*)fbb.GetBufferPointer(), fbb.GetSize());
	file.close();*/

	FILE* hFile = fopen(m_iszOutfile.c_str(), "wb");
	if (!hFile) {
		PRINT_ERR("Cannot open outfile!");
		return;
	}

	fwrite(fbb.GetBufferPointer(), 1, fbb.GetSize(), hFile);

	fclose(hFile);
}
