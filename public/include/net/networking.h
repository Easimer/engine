#pragma once

#include <cstdint>
#include <math/vector.h>
#include <schemas/edict_generated.h>
#include <vector>

#if defined(PLAT_LINUX)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#elif defined(PLAT_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

namespace net {
	const size_t max_edicts = 2048;
	const size_t stored_updates = 4;
	const unsigned short port = 38576;
	const size_t max_players = 256;
#if defined(PLAT_WINDOWS)
	using socket_t = size_t;
	const socket_t invalid_socket = INVALID_SOCKET;
	const socket_t socket_error = SOCKET_ERROR;
#elif defined(PLAT_LINUX)
	using socket_t = int;
	const socket_t invalid_socket = -1;
	const socket_t socket_error = -1;
#endif

	using client_id = uint8_t;
	using ent_id = uint32_t;
	using frame = size_t;
	
	// N = networked, C = used by client only, S = used by server only
	struct edict_t {
		bool active = false;				// [S] is the entity active/networked
		bool updated = false;				// [S] was the entity updated
		math::vector3<float> position;		// [N] position at time of last_update
		math::vector3<float> iposition;		// [C] interpolated position
		math::vector3<float> velocity;		// [C] velocity of entity
		float rotation[16];					// [N] rotation transformation matrix of entity (unused atm)
		math::vector3<float> rotation2;		// [N] rotation angles of entity (temp)
		char modelname[128];				// [N] model name of entity
		float last_update;					// [N] time the entity was last updated
	};

	struct world_update {
		net::frame id;
		std::vector<edict_t> updated_entities;
	};

	struct client_update {
		float rotation[16];
		float speed_fwd;
		float speed_side;
		float speed_up;
		unsigned buttons;
	};

	inline void close_socket(socket_t s) {
#if defined(PLAT_WINDOWS)
		closesocket(s);
#elif defined(PLAT_LINUX)
		close(s);
#endif
	}
}

inline bool operator==(const sockaddr_in& lhs, const sockaddr_in& rhs) {
	return (lhs.sin_port == rhs.sin_port) && (memcmp(&lhs.sin_addr, &rhs.sin_addr, sizeof(lhs.sin_addr)) == 0);
}
