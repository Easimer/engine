#include "stdafx.h"
#include <enl/cmdline.h>
#include "entsys.h"
#include "renderer.h"

int main(int argc, char** argv)
{
	CMDLINE_INIT();

	//PRINT_DBG(gpCmdline->GetExecName());
	// init globals
	ASSERT(gpGlobals);

	gpGlobals->pEntSys = new entsys();
	gpGlobals->pRenderer = new renderer();
	//

	gpGlobals->pRenderer->open_window("engine", 1280, 720, false);



	gpGlobals->pRenderer->close_window();

	// deinit globals
	delete gpGlobals->pEntSys;
	delete gpGlobals->pEntSys;

	//CMDLINE_SHUTDOWN(); // CRASH HERE

	return 0;
}
