#pragma once

#include <net/networking.h>

class server_list {
public:
	server_list();
	void add_server(const sockaddr_in6& srv);
	void render();
private:
	std::vector<sockaddr_in6> m_servers;
	sockaddr_in6* m_pSelected;
};
