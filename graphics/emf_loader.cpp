#include "stdafx.h"
#include <cstdio>
#include <print.h>
#include <gfx/emf_loader.h>
#include <flatbuffers/flatbuffers.h>
#include <schemas/model_generated.h>
#include <enl/benchmark.h>
#include <enl/assert.h>

using namespace gfx;

emf_loader::emf_loader(const std::string & filename)
{
	FILE* hFile = fopen(filename.c_str(), "rb");
	if (!hFile) {
		PRINT_ERR("emf_loader: cannot open model " << filename);
		return;
	}

	benchmark bm;
	bm.start();

	fseek(hFile, 0, SEEK_END);
	size_t nSiz = ftell(hFile);
	size_t nRead = 0;
	fseek(hFile, 0, SEEK_SET);

	char* pBuf = new char[nSiz];

	nRead = fread(pBuf, 1, nSiz, hFile);

	if (nRead != nSiz) {
		PRINT_ERR("emf_loader: fread failed: expected " << nSiz << " bytes, got " << nRead);
		delete[] pBuf;
		fclose(hFile);
		return;
	}

	auto verifier = flatbuffers::Verifier((const uint8_t*)pBuf, nSiz);

	if (Schemas::Model::VerifyModelBuffer(verifier))
	{
		auto fb_model = Schemas::Model::GetModel(pBuf);
		ASSERT(fb_model->Verify(verifier));

		const char* pszMat = fb_model->material()->c_str();
		m_model.material = pszMat;

		ASSERT(fb_model->triangles());
		ASSERT(fb_model->skeleton());
		ASSERT(fb_model->animations());

		for (auto it = fb_model->triangles()->begin(); it != fb_model->triangles()->end(); ++it) {
			gfx::triangle triangle;
			for (auto vex_it = (*it)->vertices()->begin(); vex_it != (*it)->vertices()->end(); ++vex_it) {
				gfx::triangle_vertex vertex;
				vertex.iBoneID		= (*vex_it)->bone();
				vertex.pos[0]		= (*vex_it)->pos()->x();
				vertex.pos[1]		= (*vex_it)->pos()->y();
				vertex.pos[2]		= (*vex_it)->pos()->z();
				vertex.normal[0]	= (*vex_it)->normal()->x();
				vertex.normal[1]	= (*vex_it)->normal()->y();
				vertex.normal[2]	= (*vex_it)->normal()->z();
				vertex.u = (*vex_it)->u();
				vertex.v = (*vex_it)->v();
				triangle.push_back(vertex);
			}
			m_model.triangles.push_back(triangle);
		}

		for (auto it = fb_model->skeleton()->begin(); it != fb_model->skeleton()->end(); ++it) {
			gfx::bone bone;
			bone.iID = (*it)->id();
			bone.iParentID = (*it)->parent();
			strncpy(bone.szName, (*it)->name()->c_str(), SMD_MAX_BONE_NAME_SIZ);
			m_model.bones.push_back(bone);
		}

		for (auto it = fb_model->animations()->begin(); it != fb_model->animations()->end(); ++it) {
			std::vector<gfx::keyframe> keyframes;
			auto name = (*it)->name()->str();
			for (auto anim_it = (*it)->keyframes()->begin(); anim_it != (*it)->keyframes()->end(); ++anim_it) {
				gfx::keyframe kf;
				kf.iFrame = (*anim_it)->time();
				for (auto bones_it = (*anim_it)->bonestates()->begin(); bones_it != (*anim_it)->bonestates()->end(); ++bones_it) {
					gfx::bone_state bs;

					bs.pos[0] = (*bones_it)->pos()->x();
					bs.pos[1] = (*bones_it)->pos()->y();
					bs.pos[2] = (*bones_it)->pos()->z();

					bs.rot[0] = (*bones_it)->rot()->x();
					bs.rot[1] = (*bones_it)->rot()->y();
					bs.rot[2] = (*bones_it)->rot()->z();

					kf.bones.push_back({ (int)(*bones_it)->bone(), bs });
				}
				keyframes.push_back(kf);
			}
			m_model.animations.emplace(name, keyframes);
		}
	}
	else {
		PRINT_ERR("Failed to verify model " << filename);
	}
	delete[] pBuf;
	bm.end();
	PRINT_DBG("Model loaded under " << bm.elapsed() << " secs");
}

model emf_loader::get_model() const
{
	return m_model;
}
