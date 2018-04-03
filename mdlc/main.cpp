#include "stdafx.h"
#include <enl/cmdline.h>
#include "qc_parser.h"
#include <gfx/smd_loader.h>
#include "emf_writer.h"

#define CHK_QC(param) if(qcp.is_cmd(param))

int main(int argc, char** argv)
{
	CMDLINE_INIT();

	if (argc < 2) {
		PRINT_ERR("Usage: " << argv[0] << " input.qc");
		return 1;
	}

	const char* pszQC = gpCmdline->GetArgument();

	if (pszQC) {
		mdlc::qc_parser qcp(pszQC);
		emf_writer emfw;
		CHK_QC("outfile") {
			auto iszOutPath = qcp.get_string("outfile");
			emfw.set_outfile(iszOutPath);
		}
		CHK_QC("mesh") {
			auto iszMeshPath = qcp.get_string("mesh");
			gfx::smd_loader smdp_mesh(iszMeshPath);
			if (smdp_mesh.fail()) {
				PRINT_ERR("Failed to parse mesh!");
				goto fail;
			}
			gfx::model mdl_mesh = smdp_mesh.get_model();
			if (mdl_mesh.material.size() == 0) {
				PRINT_ERR("Mesh uses no material!");
				return 1;
			}
			emfw.set_mesh(mdl_mesh.triangles);
			emfw.add_bones(mdl_mesh.bones);
			emfw.set_material(mdl_mesh.material);

			PRINT("Mesh: " << iszMeshPath);
		}
		CHK_QC("collision") {
			auto iszCollPath = qcp.get_string("collision");
			gfx::smd_loader smdp_collision(iszCollPath);
			if (smdp_collision.fail()) {
				PRINT_ERR("Failed to parse collision mesh!");
				goto fail;
			}
			emfw.set_collmesh(smdp_collision.get_model().triangles);

			PRINT("Collision mesh: " << iszCollPath);
		}

		for (auto& cmd : qcp.get_commands()) {
			if (cmd.find("animation_") == 0) {
				std::string name = cmd.substr(cmd.find('_') + 1);
				PRINT("Processing animation " << name);

				// TODO:
				emfw.add_animation(name, std::vector<gfx::keyframe>());
			}
		}
		PRINT("Writing to outfile \"" << emfw.get_outfile() << '\"');
		emfw.write();
	}

	fail:

	CMDLINE_SHUTDOWN();
}