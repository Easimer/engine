#pragma once

#include "logger_shared.h"

class logger_client {
public:
	logger_client(const char* szServer);
	~logger_client();

	void write(const logger::message& msg);
	void write(logger::severity severity, const char* szApp, const char* szMsg);

	logger_client(const logger_client&) = delete;
	void operator=(const logger_client&) = delete;
private:
	int m_iSocket;
	uint64_t m_iCounter;

	uint32_t m_iAddress;
	uint16_t m_iPort;
};
