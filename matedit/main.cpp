#include "stdafx.h"
#include <iostream>
#include <thread>
#include <enl/cmdbuf.h>
#include <ctime>
#include <vector>
#include <gfx/gfx.h>
#include <gfx/shader_program.h>

#include "window_preview.h"
#include "window_properties.h"

#include <qc.h>

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

	window_preview w_preview;
	window_properties w_props;

	gpGfx->add_window(&w_preview);
	gpGfx->add_window(&w_props);

	while (true) {
		gpGfx->begin_frame();
		gpGfx->draw_windows();

		

		gpGfx->end_frame();

		if (gpGfx->handle_events())
			break;
	}

	if (!gpGfx->shutdown())
		return -1;
	return 0;
}
