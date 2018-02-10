#pragma once

#include <SDL2/SDL.h>
#include "cmdbuf.h"

CMDBUF_BEGIN_CMD(drawcmd_t)
	size_t iModelID;
	vec vecPosition;
	float flRotation;
CMDBUF_END_CMD(drawcmd_t)

CMDBUF_DEF(renderer_cmdbuf, drawcmd_t, ENTSYS_MAX_ENTITIES, true, false);

class renderer {
public:
	void open_window(const char* szTitle, int nWidth, int nHeight, bool bFullscreen);
	void close_window();
	void render();
	bool init_gl();
	void shutdown_gl();

	void draw_model(size_t iModelID, vec& vecPosition, float flRotation);

private:
	renderer_cmdbuf m_cmdbuf;

	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;
};
