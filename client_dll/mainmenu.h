#pragma once

#include <array>

class mainmenu {
public:
	enum exitcode {
		EMMENU_OK = 0,
		EMMENU_START_LOCAL_GAME,
		EMMENU_JOIN_REMOTE_GAME,
		EMMENU_QUIT_GAME,
		EMMENU_MAX
	};

	enum buttons {
		EMMENUBTN_LOCAL_GAME = 0,
		EMMENUBTN_REMOTE_GAME,
		EMMENUBTN_QUIT_GAME,
		EMMENUBTN_MAX
	};

	mainmenu();
	exitcode tick();
	
private:
	
};
