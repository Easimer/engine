#include "stdafx.h"
#include <enl/cmdline.h>
#include "entsys.h"

int main(int argc, char** argv)
{
	CMDLINE_INIT();

	//PRINT_DBG(gpCmdline->GetExecName());
	// init globals
	ASSERT(gpGlobals);

	gpGlobals->pEntSys = new entsys();

	//

	// deinit globals
	delete gpGlobals->pEntSys;

	//CMDLINE_SHUTDOWN(); // CRASH HERE

	return 0;
}
