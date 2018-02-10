#include "stdafx.h"
#include "renderer.h"

#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

void renderer::open_window(const char * szTitle, int nWidth, int nHeight, bool bFullscreen)
{
	RESTRICT_THREAD_RENDERING;
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	m_pWindow = SDL_CreateWindow(szTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, nWidth, nHeight, (bFullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	ASSERT_SDL2(m_pWindow);
	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	ASSERT_SDL2(m_pRenderer);
}

void renderer::close_window()
{
	RESTRICT_THREAD_RENDERING;
	if (m_pRenderer)
		SDL_DestroyRenderer(m_pRenderer);
	if (m_pWindow)
		SDL_DestroyWindow(m_pWindow);
}

void renderer::render()
{
	RESTRICT_THREAD_RENDERING;
	SDL_GL_SwapWindow(m_pWindow);
}

bool renderer::init_gl()
{
	RESTRICT_THREAD_RENDERING;
	return false;
}

void renderer::shutdown_gl()
{
	RESTRICT_THREAD_RENDERING;
}

void renderer::draw_model(size_t iModelID, vec & vecPosition, float flRotation)
{
	RESTRICT_THREAD_RENDERING;
	while (!m_cmdbuf.IsClosed()) {}
	
	drawcmd_t* pCommands;
	size_t nCommands;

	m_cmdbuf.BeginRead(&pCommands, &nCommands);

	m_cmdbuf.EndRead();
}
