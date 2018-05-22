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
	using socklen_t = int;
	const socket_t invalid_socket = INVALID_SOCKET;
	const socket_t socket_error = SOCKET_ERROR;
#elif defined(PLAT_LINUX)
	using socket_t = int;
	using socklen_t = ::socklen_t;
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
		math::vector3<float> velocity;		// [C] velocity of entity, calculated from old and current position
		math::vector3<float> iposition;		// [C] interpolated position, calculated by applying velocity to position
		math::vector3<float> iacceleration;	// [C] interpolated acceleration, calculated from old and current velocity
		math::vector3<float> ivelocity;		// [C] interpolated velocity, calculated first from velocity, then by applying iacceleration to ivelocity
		float rotation[16];					// [N] rotation transformation matrix of entity
		float irotation[16];				// [C] interpolated rotation transformation matrix of entity
		//math::vector3<float> rotation2;	// [N] rotation angles of entity (temp)
		char modelname[128];				// [N] model name of entity
		float last_update;					// [N] time the entity was last updated
		float angular_vel[3];				// [C] angular velocity of the model

		void reset() {
			active = updated = false;
			position = velocity = iposition = iacceleration = ivelocity = vec3(0, 0, 0);
			for (size_t i = 0; i < 16; i++) rotation[i] = 0;
			rotation[0] = 1; rotation[5] = 1; rotation[10] = 1; rotation[15] = 1;
			modelname[0] = 0;
			last_update = 0.f;
			angular_vel[0] = 0; angular_vel[1] = 0; angular_vel[2] = 0; angular_vel[3] = 0;
		}
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
