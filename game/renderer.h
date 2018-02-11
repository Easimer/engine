#pragma once

#include <SDL2/SDL.h>
#include "cmdbuf.h"
#include <model.h>

//
// model draw cmdbuf
//

CMDBUF_BEGIN_CMD(drawcmd_t)
	size_t iModelID;
	vec vecPosition;
	float flRotation;
CMDBUF_END_CMD(drawcmd_t)

CMDBUF_DEF(renderer_drawmdl_cmdbuf, drawcmd_t, ENTSYS_MAX_ENTITIES, true, false);

//
// model load cmdbuf
//

#define LDMDL_CMD_MAX_FN 64
typedef size_t model_id;

CMDBUF_BEGIN_CMD(ldmdl_cmd_t)
	char szFilename[LDMDL_CMD_MAX_FN];
CMDBUF_END_CMD(ldmdl_cmd_t)

CMDBUF_DEF(renderer_ldmdl_cmdbuf, ldmdl_cmd_t, 512, true, false);

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
	void load_models(std::vector<std::string> filenames, std::vector<model_id>& model_ids);
	void draw_model(size_t iModelID, vec& vecPosition, float flRotation);

	model_id upload_model(const model&);

	void model_load_loop();

private:
	renderer_drawmdl_cmdbuf m_cmdbuf;
	renderer_ldmdl_cmdbuf m_ldmdl_cmdbuf;

	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;
	SDL_GLContext m_pGLContext;

	bool m_bLoading = false;

	size_t m_iLoadedModelID = 0;
};
