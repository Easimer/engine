#include "stdafx.h"
#include <enl/cmdline.h>

int main(int argc, char** argv)
{
	CMDLINE_INIT();
	PRINT_DBG(gpCmdline->GetExecName());
	CMDLINE_SHUTDOWN();
	return 0;
}
