#include "stdafx.h"
#include <enl/platform.h>
#include <enl/assert.h>
#if defined(PLAT_WINDOWS)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(PLAT_LINUX)
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#endif
#include "logger_shared.h"
#include "logger_server.h"
#include <iostream>
#include <ctime>

logger_server::logger_server()
{
#if defined(PLAT_WINDOWS)
	WSADATA wsadata;
	unsigned iRes = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iRes != 0)
	{
		std::cerr << "TCPClient: WSAStartup failed with error " << iRes << std::endl;
		return;
	}
#endif

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(516);
	local.sin_addr.s_addr = INADDR_ANY;
	m_iSocket = socket(AF_INET, SOCK_DGRAM, 0);
	bind(m_iSocket, (sockaddr*)&local, sizeof(local));
}

logger_server::~logger_server()
{
#if defined(PLAT_WINDOWS)
	closesocket(m_iSocket);
	WSACleanup();
#else
	close(m_iSocket);
#endif
}

#if defined(PLAT_WINDOWS)
#define _SOCKLEN int
#else
#define _SOCKLEN socklen_t
#endif

void logger_server::serve()
{
	while (1) {
		struct sockaddr_in from;
		_SOCKLEN nFromSiz = sizeof(from);

		char buf[576];

		logger::message* pMsg = (logger::message*)buf;

		memset(buf, 0, 576);
		if (recvfrom(m_iSocket, buf, 576, 0, (sockaddr*)&from, &nFromSiz)) {
			char timestampbuf[64];
			strftime(timestampbuf, 64, "%Y-%m-%d %H:%M:%S", localtime((const time_t*)&pMsg->timestamp));
			printf("[%s] %s(%s): %s\n", timestampbuf, pMsg->application, logger::strseverity((logger::severity)pMsg->severity), pMsg->content);
			fflush(stdout);
		}
	}
}

bool logger_server::serve_one(logger::message& msg)
{
	struct sockaddr_in from;
	fd_set fds;
	struct timeval tv;
	_SOCKLEN nFromSiz = sizeof(from);

	FD_ZERO(&fds);
	FD_SET(m_iSocket, &fds);
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	memset(&msg, 0, logger::nMaxMessageSiz);

	int n = select(m_iSocket, &fds, NULL, NULL, &tv);
	if (n == 0)
		return false;
	
	if (recvfrom(m_iSocket, (char*)&msg, 576, 0, (sockaddr*)&from, &nFromSiz)) {
		char timestampbuf[64];
		strftime(timestampbuf, 64, "%Y-%m-%d %H:%M:%S", localtime((const time_t*)&msg.timestamp));
		printf("[%s] %s(%s): %s\n", timestampbuf, msg.application,	logger::strseverity((logger::severity)msg.severity), msg.content);
		fflush(stdout);
		return true;
	}

	return false;
}
