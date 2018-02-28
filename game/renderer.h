#pragma once

#include <SDL2/SDL.h>
#include <enl/cmdbuf.h>
#include <model.h>
#include "shader_program.h"
#include <map>
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

//
// model draw cmdbuf
//

CMDBUF_BEGIN_CMD(drawcmd_t)
	model_id iModelID;
	vec vecPosition;
	glm::mat4 matRotation;
	float flScale = 1;
CMDBUF_END_CMD(drawcmd_t)

CMDBUF_DEF(renderer_drawmdl_cmdbuf, drawcmd_t, ENTSYS_MAX_ENTITIES, false, false);

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

enum mdl_vertexattrib {
	MDL_VBO_POSITION	= 0, // contains vertex position (3 GLfloats)
	MDL_VBO_NORMAL		= 1, // contains vertex normal (3 GLfloats)
	MDL_VBO_UV			= 2, // contains vertex uv (2 GLfloats)
	MDL_VBO_BONE		= 3, // contains bone ID (1 UNSIGNED_INT)
	MDL_VBO_MAT			= 4, // contains material IDs
	MDL_VBO_MAX			= 5
};

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
	
	// DEPRECATED
	void draw_model(size_t iModelID, vec& vecPosition, float flRotation);
	void draw_models(std::vector<drawcmd_t>& cmds);

	void load_shader(const char* szFilename);

	model_id upload_model(const model&);

	uint32_t load_texture(const std::string& filename);

	void load_loop();

	void update_camera(glm::vec3& pos, glm::mat4& rot);

	void init_gui();
	void shutdown_gui();

	bool waiting_for_draw() const {
		return m_cmdbuf.is_empty();
	}

	void draw_debug_tools();

private:
	renderer_drawmdl_cmdbuf m_cmdbuf;
	renderer_load_cmdbuf m_gfx_ld_cmdbuf;

	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;
	SDL_GLContext m_pGLContext;

	bool m_bLoading = true;

	size_t m_iLoadedModelID = 0;

	// list of shaders
	// TODO: make this a map<string [name], shader_program*>
	std::vector<shader_program*> m_vecPrograms;

	std::map<std::string, model_id> m_mapModels;

	glm::mat4 m_matProj;
	glm::mat4 m_matView;

	std::map<model_id, size_t> m_model_vertexcount;
	// maps texture filename to GL texture ID
	std::map<std::string, uint32_t> m_map_texture_name;
	/*	maps texture ids to texture units of a model
		TODO: doc this better
		i.e. mapping of three textures to three tex units of a model:

					<- GL_TEXTURE0 <- 5
		MODEL(0)	<- GL_TEXTURE1 <- 13
					<- GL_TEXTURE2 <- 1
	*/
	std::map<model_id, std::vector<uint32_t>> m_mapTextures;

	// FPS counting
	size_t m_nFrames = 0;
	float m_flNow = 0, m_flLast = 0;
	float m_flFPS = 0;

	// Debug GUI state
	bool m_bShowRendererDebug = false;
	bool m_bShowStatistics = false;
	bool m_bShowInspector = false;
	size_t m_iCurEnt = 0;
};
