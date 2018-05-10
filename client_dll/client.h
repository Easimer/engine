#pragma once

#include "iclient.h"
#include <net/client.h>
#include <memory>

class client : public iclient {
public:
	virtual void init(const char* pszHostname, const char* pszUsername) override;
	virtual void shutdown() override;
	virtual bool is_shutdown() override;
private:
	std::unique_ptr<net::client> m_pClient;
};
