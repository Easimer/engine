#include "stdafx.h"
#include "renderer.h"

#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <assert_opengl.h>

#include "qc_parser.h"
#include "smd_parser.h"

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

	// Draw models
	while (!m_cmdbuf.IsClosed());
	drawcmd_t* pCommands;
	size_t nCommands;
	m_cmdbuf.BeginRead(&pCommands, &nCommands);
	m_cmdbuf.EndRead();

	SDL_GL_SwapWindow(m_pWindow);
}

bool renderer::init_gl()
{
	RESTRICT_THREAD_RENDERING;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	m_pGLContext = SDL_GL_CreateContext(m_pWindow);

	if (!m_pGLContext)
	{
		ASSERT_SDL2(0);
		return false;
	}

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	PRINT_DBG("GL Vendor: " << glGetString(GL_VENDOR));
	PRINT_DBG("GL Renderer: " << glGetString(GL_RENDERER));
	PRINT_DBG("GL Version: " << glGetString(GL_VERSION));

	SDL_GL_SetSwapInterval(1);

	int nWidth, nHeight;
	SDL_GetWindowSize(m_pWindow, &nWidth, &nHeight);

	glViewport(0, 0, nWidth, nHeight);
	ASSERT_OPENGL();
	glClearColor(0.39215686274f, 0.58431372549f, 0.9294117647f, 1.0f);
	ASSERT_OPENGL();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_MULTISAMPLE);

	return true;
}

void renderer::shutdown_gl()
{
	RESTRICT_THREAD_RENDERING;
	if (m_pGLContext)
		SDL_GL_DeleteContext(m_pGLContext);
}


model_id renderer::load_model( const char * szFilename)
{
	RESTRICT_THREAD_LOGIC;
	ldmdl_cmd_t c;
	strncpy(c.szFilename, szFilename, LDMDL_CMD_MAX_FN);
	m_ldmdl_cmdbuf.BeginWrite();
	m_ldmdl_cmdbuf.Write(c);
	m_ldmdl_cmdbuf.EndWrite();
	// wait for renderer thread to upload the model
	while (m_iLoadedModelID == 0);
	// save model id
	model_id ret = m_iLoadedModelID;
	// signal renderer thread to load next model
	m_iLoadedModelID = 0;
	return ret;
}
void renderer::draw_model(size_t iModelID, vec & vecPosition, float flRotation)
{
	RESTRICT_THREAD_LOGIC;
	drawcmd_t c = { iModelID, vecPosition, flRotation };
	m_cmdbuf.BeginWrite();
	m_cmdbuf.Write(c);
	m_cmdbuf.EndWrite();
}

void renderer::model_load_loop()
{
	PRINT_DBG("renderer::model_load_loop entered");
	while (m_bLoading)
	{
		while (m_bLoading && (m_ldmdl_cmdbuf.IsEmpty()));
		if (!m_ldmdl_cmdbuf.IsEmpty())
		{
			ldmdl_cmd_t* pCommands;
			size_t nCommands;
			m_ldmdl_cmdbuf.BeginRead(&pCommands, &nCommands);
			while (nCommands--)
			{
				mdlc::smd_parser parser(pCommands->szFilename);
				
				pCommands++;
			}
			m_ldmdl_cmdbuf.EndRead();
		}
	}
	PRINT_DBG("renderer::model_load_loop ended");
}
