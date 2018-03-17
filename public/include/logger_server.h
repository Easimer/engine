#pragma once

#include <logger_shared.h>

class logger_server {
public:
	logger_server();
	~logger_server();

	void serve();
	bool serve_one(logger::message& msg);

	logger_server(const logger_server&) = delete;
	void operator=(const logger_server&) = delete;

private:
	int m_iSocket;
};