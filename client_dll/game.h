#pragma once

#include <net/client.h>
#include <gfx/gfx.h>
#include <gfx/camera.h>
#include <array>
#include "input.h"
#include "events.h"

#include <gfx/pipeline/pipeline.h>

class game {
public:
	game();
	bool paused() const { return m_bPaused; }
	void paused(bool b){ m_bPaused = b; }

	void connect(const char* pszHostname, const char* pszUsername);
	void connect(const sockaddr_in6& addr, const char* pszUsername);

	void disconnect();
	bool tick();

	std::shared_ptr<net::client> get_socket() const { return m_pNetClient; }

private:
	bool m_bPaused;
	std::shared_ptr<net::client> m_pNetClient;
	gfx::camera m_camera;
	// Stores model ID associated with edict index
	std::array<gfx::model_id, net::max_edicts> m_model_cache;
	glm::mat4 m_proj;
	input m_input;
	event_handler m_evhandler;
	gfx::shared_fb m_fb;
	gfx::pipeline::pipeline m_pipeline;
};
