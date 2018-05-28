#pragma once
#include "iserver.h"
#include <net/server.h>

constexpr const int server_tickrate = 30; // Hz

class server : public iserver {
public:
	virtual void init() override;
	virtual void shutdown() override;
	virtual bool is_shutdown() override { return m_shutdown; }
	virtual const char* name() const override;
	void client_input_handler(const net::client_desc& client, size_t edict, uint64_t cli_tick, const char* pszCommand);
private:
	std::unique_ptr<net::server> m_server;
	bool m_shutdown = false;
	std::thread m_thread_logic;
};
