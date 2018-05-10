#include "stdafx.h"
#include "iclient.h"
#include "client.h"

extern "C" {
	ENL_EXPORT iclient* client_dll_init() {
		return new client();
	}

	ENL_EXPORT void client_dll_shutdown(iclient* p) {
		delete static_cast<client*>(p);
	}
}

void client::init(const char* pszHostname, const char* pszUsername)
{
	std::flush(std::cout);
	PRINT("client::init");
	m_pClient = std::make_unique<net::client>(pszHostname, pszUsername);
	for (int i = 0; i < 5; i++) {
		if (m_pClient->connected())
			break;
		std::this_thread::sleep_for(std::chrono::duration<float>(1.5f));
		m_pClient->attempt_connect();
	}
	if (!m_pClient->connected()) {
		PRINT_ERR("Failed to connect to listen server!");
	}
}

void client::shutdown()
{
	m_pClient->disconnect();
}

bool client::is_shutdown()
{
	return false;
}
