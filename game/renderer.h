#pragma once

#include <SDL2/SDL.h>
#include "cmdbuf.h"
#include <model.h>
#include "shader_program.h"
#include <map>
#include <glm/mat4x4.hpp>

//
// model draw cmdbuf
//

CMDBUF_BEGIN_CMD(drawcmd_t)
	model_id iModelID;
	vec vecPosition;
	float flRotation;
CMDBUF_END_CMD(drawcmd_t)

CMDBUF_DEF(renderer_drawmdl_cmdbuf, drawcmd_t, ENTSYS_MAX_ENTITIES, true, false);

//
// model load cmdbuf
//

#define GFX_LD_CMD_MAX_FN 64

enum gfx_ld_cmd_type {
	GFX_LD_T_MDL = 0,
	GFX_LD_T_SHADER = 1,
	GFX_LD_T_MAX = 2
};

CMDBUF_BEGIN_CMD(gfx_load_cmd_t)
	gfx_ld_cmd_type type;
	char szFilename[GFX_LD_CMD_MAX_FN];
CMDBUF_END_CMD(gfx_load_cmd_t)

CMDBUF_DEF(renderer_load_cmdbuf, gfx_load_cmd_t, 512, true, false);

#define MDL_VBO_POSITION	0 // contains vertex position (3 GLfloats)
#define MDL_VBO_NORMAL		1 // contains vertex normal (3 GLfloats)
#define MDL_VBO_UV			2 // contains vertex uv (2 GLfloats)
#define MDL_VBO_BONE		3 // contains bone ID (1 UNSIGNED_INT)
#define MDL_VBO_MAT			4 // contains texture sampler ID (1 UNSIGNED_INT)
#define MDL_VBO_MAX			5

class renderer {
public:
	void open_window(const char* szTitle, int nWidth, int nHeight, bool bFullscreen);
	void close_window();
	void render();
	bool init_gl();
	void shutdown_gl();

	void begin_load() {
		RESTRICT_THREAD_LOGIC;
		if (m_bLoading)
			return;
		m_bLoading = true;
	}

	void end_load() {
		RESTRICT_THREAD_LOGIC;
		if (!m_bLoading)
			return;
		m_bLoading = false;
	}

	model_id load_model(const char * szFilename);
	void load_models(std::vector<std::string> filenames);
	
	void draw_model(size_t iModelID, vec& vecPosition, float flRotation);
	void draw_models(std::vector<drawcmd_t>& cmds);

	void load_shader(const char* szFilename);

	model_id upload_model(const model&);

	void load_loop();

	void update_camera(vector& pos, vector& rot);

	void _load_test_model();

private:
	renderer_drawmdl_cmdbuf m_cmdbuf;
	renderer_load_cmdbuf m_gfx_ld_cmdbuf;

	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;
	SDL_GLContext m_pGLContext;

	bool m_bLoading = true;

	size_t m_iLoadedModelID = 0;

	std::vector<shader_program*> m_vecPrograms;

	std::map<std::string, model_id> m_mapModels;

	glm::mat4 m_matProj;
	glm::mat4 m_matView;

	std::map<model_id, size_t> m_model_vertexcount;
};
