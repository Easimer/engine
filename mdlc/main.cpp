#include "stdafx.h"
#include <enl/cmdline.h>
#include "qc_parser.h"
#include "smd_parser.h"
#include "emf_writer.h"

#define CHK_QC(param) if(qcp.is_cmd(param))

int main(int argc, char** argv)
{
	CMDLINE_INIT();

	if (argc < 2)
		return 1;

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
			mdlc::smd_parser smdp_mesh(iszMeshPath);
			if (smdp_mesh.fail()) {
				PRINT_ERR("Failed to parse mesh!");
				goto fail;
			}
			model mdl_mesh = smdp_mesh.get_model();
			emfw.set_mesh(mdl_mesh.triangles);
			emfw.add_bones(mdl_mesh.bones);

			PRINT("Mesh: " << iszMeshPath);
		}
		CHK_QC("collision") {
			auto iszCollPath = qcp.get_string("collision");
			mdlc::smd_parser smdp_collision(iszCollPath);
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
			}
		}
	}



	fail:

	CMDLINE_SHUTDOWN();
}