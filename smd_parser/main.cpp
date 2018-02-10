#include "stdafx.h"
#include <enl/cmdline.h>
#include <enl/benchmark.h>

int main(int argc, char** argv)
{
	CMDLINE_INIT();

	const char* szFilename = NULL;

	if (argc < 2) {
		szFilename = "Debug/cowboy_hat.smd";
	} else {
		szFilename = gpCmdline->GetArgument();
	}

	double s = 0;

	benchmark bm;
	bm.start();
	mdlc::smd_parser parser(szFilename);
	s += bm.end();

	PRINT_DBG("SMD parsed under " << s << " secs");

	// wait for key
	PRINT("Waiting for key...");
	std::string a;
	std::cin >> a;
	return 0;
}
