#pragma once

#include <array>
#include <net/networking.h>
#include <net/client.h>

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

protected:
	size_t discover_servers();

private:
	std::vector<sockaddr_in6> m_servers;
	sockaddr_in6* m_pSelected;
	bool m_bShowServerBrowser = false;
	std::unique_ptr<net::server_discovery> m_pSDClient;
};
