#include "stdafx.h"
#include <iostream>
#include <thread>
#include <enl/cmdbuf.h>
#include <ctime>
#include <vector>
#include <gfx/gfx.h>
#include <gfx/shader_program.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include "window_model.h"
#include "window_properties.h"

gfx::model_id giCurrentModel = 0;

static bool bShutdown = false;

bool set_workdir()
{
#if defined(PLAT_WINDOWS)
	DWORD ftyp = GetFileAttributesA("data");
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY && ftyp != INVALID_FILE_ATTRIBUTES) {
		PRINT_DBG("Already running from game rootdir!");
		return true;
	}

	SetCurrentDirectory("..");
	ftyp = GetFileAttributesA("data");
	if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY) || ftyp == INVALID_FILE_ATTRIBUTES) {
		PRINT_ERR("Switched to parent dir, but data/ doesn't exist!");
		return false;
	}
	return true;
#elif defined(PLAT_LINUX)
	if (chdir("..") == 0) {
		DIR* pDir = opendir("data");
		if (pDir) {
			closedir(pDir);
			return true;
		}
		else {
			PRINT_ERR("Switched to parent dir, but data/ doesn't exist!");
		}
	}
	else {
		PRINT_ERR("Couldn't switch to parent dir!");
	}
	return false;
#endif
}

int main(int argc, char** argv) {

	if (!set_workdir()) {
		PRINT_ERR("set_workdir failure");
	}

	if (!gpGfx->init("mdlviewer", 800, 600))
		return -1;

	gpGfx->load_shader("data/shaders/model_dynamic.qc");

	window_model w_model;
	window_properties w_props;

	gpGfx->add_window(&w_model);
	gpGfx->add_window(&w_props);

	glm::mat4 proj = glm::perspective(glm::radians(90.f), (float)gpGfx->width() / (float)gpGfx->height(), 0.0f, 1000.0f);
	//glm::mat4 proj = glm::ortho(0.0f, (float)gpGfx->width(), (float)gpGfx->height(), 0.0f, 0.0f, 1000.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -1));

	float rot = 0.0f;

	gfx::shader_light l;
	l.color.r = 1;
	l.color.g = 1;
	l.color.b = 1;
	l.color.a = 1;
	l.pos = math::vector3<float>(0, 0, 4);
	l.iType = gfx::shader_light_type::SLT_POINT;

	while (true) {
		gpGfx->begin_frame();
		gpGfx->draw_windows();

		if (giCurrentModel) {
			gpGfx->bind_model(giCurrentModel);

			rot += 3.1415926f / 4.0f * gpGfx->delta();
			gfx::shader_program* pShader = gpGfx->get_shader(gpGfx->use_shader());
			pShader->use_material(gpGfx->model_material(giCurrentModel));

			glm::mat4 trans(1.0);
			trans = glm::rotate(trans, rot, glm::vec3(0, 1, 0));
			
			pShader->set_local_light(l);

			pShader->set_mat_proj(glm::value_ptr(proj));
			pShader->set_mat_view(glm::value_ptr(view));
			pShader->set_mat_trans(glm::value_ptr(trans));

			gpGfx->draw_model();
		}

		gpGfx->end_frame();

		if (gpGfx->handle_events())
			break;
	}

	if (!gpGfx->shutdown())
		return -1;
	return 0;
}
