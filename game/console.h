#pragma once

#include "devgui.h"
#include "globals.h"

typedef void(*ccommand_handler)(const std::vector<std::string>& args);

struct ccommand_def {
	char szCommand[64];
	ccommand_handler* pHandler;
};

class ccommand_init {
public:
	ccommand_init(const char* szName, ccommand_handler* pHandler) {
		if (!gpGlobals->pCommandDefs) {
			gpGlobals->pCommandDefs = new ccommand_def[128];
		}
		ccommand_def* pDef = &gpGlobals->pCommandDefs[gpGlobals->iCommandDefs];
		strncpy(pDef->szCommand, szName, 64);
		pDef->pHandler = pHandler;
		gpGlobals->iCommandDefs++;
		PRINT_DBG("Defined CCommand '" << pDef->szCommand << '\'');
	}
};

// Define a console command handler quickly
#define CCOMMAND_DEF(NAME, FUN) static ccommand_init NAME ((char*)#NAME, &FUN);

void console_draw_frame();
void console_print_line(const std::string& line);
bool console_execute(const char* szCmd);
