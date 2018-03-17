#pragma once

#include <cstdint>

#undef ERROR
#undef DEBUG
namespace logger {
	enum severity {
		DEBUG = 0,
		INFO = 1,
		WARNING = 2,
		ERROR = 3,
		EMERGENCY = 4,
		MAX = 5
	};

	const size_t nMaxApplicationSiz = 16;
	const size_t nMaxContentSiz = 256;
	const size_t nMaxMessageSiz = 576;

	struct message {
		// message id
		uint64_t id;
		// message timestamp (UTC, seconds since 1970/01/01 00:00:00)
		uint64_t timestamp;
		// message severity
		uint32_t severity;
		// application identifier
		char application[nMaxApplicationSiz];
		// message content
		char content[nMaxContentSiz];
	};

	static_assert(sizeof(message) < nMaxMessageSiz, "logger::message is too big");

	inline const char* strseverity(severity s) {
		switch (s) {
		case DEBUG:
			return "DEBUG";
		case INFO:
			return "INFO";
		case WARNING:
			return "WARNING";
		case ERROR:
			return "ERROR";
		case EMERGENCY:
			return "EMERGENCY";
		default:
			return "UNKNOWN";
		}
	}
};


