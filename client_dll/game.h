#pragma once

#include <net/client.h>
#include <gfx/gfx.h>
#include <gfx/camera.h>
#include <array>

class game {
public:
	game();
	bool paused() const { return m_bPaused; }
	void paused(bool b){ m_bPaused = b; }

	void connect(const char* pszHostname, const char* pszUsername);
	void connect(const sockaddr_in6& addr, const char* pszUsername);

	void disconnect();
	bool tick();

private:
	bool m_bPaused;
	std::unique_ptr<net::client> m_pNetClient;
	gfx::camera m_camera;
	// Stores model ID associated with edict index
	std::array<gfx::model_id, net::max_edicts> m_model_cache;
	glm::mat4 m_proj;
};
