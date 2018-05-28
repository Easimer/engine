#pragma once

#include "iclient.h"
#include <net/client.h>
#include <memory>

class client : public iclient {
public:
	virtual ~client();
	virtual void init() override;
	virtual void shutdown() override;
	virtual bool is_shutdown() override;
	virtual bool request_server() override { return m_bRequestServer; }
	virtual const char * name() const override;

	bool m_bShutdown;
	bool m_bRequestServer;
	std::shared_ptr<net::client> m_pClient;
private:
	std::thread m_client_thread;
};
