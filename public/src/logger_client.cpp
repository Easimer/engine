#include "stdafx.h"
#include "logger_client.h"
#include <iostream>
#include <ctime>

logger_client::logger_client(const char* szServer)
{
	WSADATA wsadata;
	unsigned iRes = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iRes != 0)
	{
		std::cerr << "TCPClient: WSAStartup failed with error " << iRes << std::endl;
		return;
	}

	m_iAddress = inet_addr(szServer);
	m_iPort = htons(516);

	m_iSocket = socket(AF_INET, SOCK_DGRAM, 0);

	m_iCounter = 0;
}

logger_client::~logger_client()
{
	closesocket(m_iSocket);
	WSACleanup();
}

void logger_client::write(const logger::message & msg)
{
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = m_iPort;
	server.sin_addr.s_addr = m_iAddress;

	if (sendto(m_iSocket, (char*)&msg, sizeof(msg), 0, (sockaddr*)&server, sizeof(server)) != SOCKET_ERROR) {
		std::cerr << "Sent message!" << std::endl;
	}
}

void logger_client::write(logger::severity severity, const char * szApp, const char * szMsg)
{
	logger::message msg;
	strncpy(msg.application, szApp, 16);
	msg.id = m_iCounter++;
	msg.severity = (uint32_t)severity;
	msg.timestamp = time(NULL);
	memset(msg.content, 0, 256);
	strncpy(msg.content, szMsg, 256);

	write(msg);
}
