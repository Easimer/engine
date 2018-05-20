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

	const char* selected_server_address() const { return m_szAddressBuf; }
	const char* selected_username() const { return m_szUsername; }

	void set_message(const char* msg) { m_pMsg = msg; }

protected:
	size_t discover_servers();

private:
	std::vector<net::server_entry> m_servers;
	net::server_entry* m_pSelected;
	bool m_bShowServerBrowser = false;
	std::shared_ptr<net::server_discovery> m_pSDClient;

	char m_szUsername[128];
	char m_szAddressBuf[128];
	const char* m_pMsg;
};
