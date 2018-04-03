#pragma once

#include "entsys.h"
#include <gfx/gfx.h>
#include <enl/cmdbuf.h>
#include <math/vector.h>
#include <glm/mat4x4.hpp>
#include <gfx/light.h>
#include <atomic>
#include <mutex>
#include <gfx/camera.h>

CMDBUF_BEGIN_CMD(drawcmd_t)
gfx::model_id iModelID;
math::vector3<float> vecPosition;
glm::mat4 matRotation;
float flScale = 1;
bool bMatRotationIsTransformation = false; // matRotation is the whole transformation matrix

gfx::shader_light light_local;
gfx::shader_light light_global;
CMDBUF_END_CMD(drawcmd_t)

CMDBUF_DEF(renderer_drawmdl_cmdbuf, drawcmd_t, ENTSYS_MAX_ENTITIES, false, false);

enum gfx_load_type {
	model, shader
};

class igfx {
public:
	// Game-side
	void draw_models(const std::vector<drawcmd_t>&);
	uint32_t load_model(const std::string&);
	size_t load_shader(const std::string& filename);

	void begin_load();
	void end_load();

	// Renderer side
	bool init(const char* szTitle = "engine", size_t width = 1280, size_t height = 720, size_t glVersionMajor = 3, size_t glVersionMinor = 3);
	bool shutdown();
	void update();
	void draw();

private:
	std::atomic<bool> m_bLoading = false;
	renderer_drawmdl_cmdbuf m_drawcmds;
	std::mutex m_request_load;
	std::atomic<long long signed> m_request_wait;
	std::vector<std::pair<gfx_load_type, std::string>> m_load_requests;
	glm::mat4 m_view = glm::mat4(1.0f);
	glm::mat4 m_proj = glm::mat4(1.0f);
};
