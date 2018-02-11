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


model_id renderer::load_model(const char * szFilename)
{
	RESTRICT_THREAD_LOGIC;

	/*if (m_iLoadedModelID)
	{
		PRINT_DBG("renderer::load_model: waiting to upload...");
	}*/
	while (m_iLoadedModelID) {
		Sleep(1);
	}

	ldmdl_cmd_t c;
	strncpy(c.szFilename, szFilename, LDMDL_CMD_MAX_FN);
	m_ldmdl_cmdbuf.BeginWrite();
	m_ldmdl_cmdbuf.Write(c);
	m_ldmdl_cmdbuf.EndWrite();
	//PRINT_DBG("renderer::load_model: waiting for upload...");
	// wait for renderer thread to upload the model
	while (m_iLoadedModelID == 0) {
		Sleep(1);
	}
	//PRINT_DBG("renderer::load_model: received model id!");
	// save model id
	model_id ret = m_iLoadedModelID;
	// signal renderer thread to load next model
	m_iLoadedModelID = 0;
	return ret;
}

void renderer::load_models(std::vector<std::string> filenames, std::vector<model_id>& model_ids)
{
	RESTRICT_THREAD_LOGIC;
	while (m_iLoadedModelID) {
		Sleep(1);
	}

	ldmdl_cmd_t c;
	model_ids.clear();
	m_ldmdl_cmdbuf.BeginWrite();
	for (auto& fn : filenames)
	{
		strncpy(c.szFilename, fn.c_str(), LDMDL_CMD_MAX_FN);
		m_ldmdl_cmdbuf.Write(c);
	}
	m_ldmdl_cmdbuf.EndWrite();
	for (size_t i = 0; i < filenames.size(); i++)
	{
		while (m_iLoadedModelID == 0) {
			Sleep(1);
		}
		model_ids.push_back(m_iLoadedModelID);
		m_iLoadedModelID = 0;
	}
}

void renderer::draw_model(size_t iModelID, vec & vecPosition, float flRotation)
{
	RESTRICT_THREAD_LOGIC;
	drawcmd_t c = { iModelID, vecPosition, flRotation };
	m_cmdbuf.BeginWrite();
	m_cmdbuf.Write(c);
	m_cmdbuf.EndWrite();
}

model_id renderer::upload_model(const model& mdl)
{
	RESTRICT_THREAD_RENDERING;
	GLuint iVAO;
	GLuint aiVBO[MDL_VBO_MAX];

	glGenVertexArrays(1, &iVAO); ASSERT_OPENGL();
	glBindVertexArray(iVAO); ASSERT_OPENGL();
	glGenBuffers(MDL_VBO_MAX, aiVBO); ASSERT_OPENGL();
	
	// Generate arrays
	size_t nPositionsSiz = mdl.triangles.size() * 9;
	size_t nNormalsSiz = mdl.triangles.size() * 9;
	size_t nUVsSiz = mdl.triangles.size() * 6;
	size_t nBoneIDsSiz = mdl.triangles.size() * 3;
	size_t nMatIDsSiz = mdl.triangles.size() * 3;
	float* aflPositions = new float[nPositionsSiz];
	float* aflNormals = new float[nNormalsSiz];
	float* aflUVs = new float[nUVsSiz];
	uint32_t* anBoneIDs = new uint32_t[nBoneIDsSiz];
	uint32_t* anMatIDs = new uint32_t[nMatIDsSiz];

	for (size_t iTriangle = 0; iTriangle < mdl.triangles.size(); iTriangle++)
	{
		for (size_t iVertex = 0; iVertex < 3; iVertex++)
		{
			aflPositions[iTriangle * 9 + iVertex * 3 + 0] = mdl.triangles[iTriangle].vertices[iVertex].px;
			aflPositions[iTriangle * 9 + iVertex * 3 + 1] = mdl.triangles[iTriangle].vertices[iVertex].py;
			aflPositions[iTriangle * 9 + iVertex * 3 + 2] = mdl.triangles[iTriangle].vertices[iVertex].pz;

			aflNormals[iTriangle * 9 + iVertex * 3 + 0] = mdl.triangles[iTriangle].vertices[iVertex].nx;
			aflNormals[iTriangle * 9 + iVertex * 3 + 1] = mdl.triangles[iTriangle].vertices[iVertex].ny;
			aflNormals[iTriangle * 9 + iVertex * 3 + 2] = mdl.triangles[iTriangle].vertices[iVertex].nz;

			aflUVs[iTriangle * 6 + iVertex * 2 + 0] = mdl.triangles[iTriangle].vertices[iVertex].u;
			aflUVs[iTriangle * 6 + iVertex * 2 + 1] = mdl.triangles[iTriangle].vertices[iVertex].v;

			anBoneIDs[iTriangle * 3 + iVertex] = mdl.triangles[iTriangle].vertices[iVertex].iBoneID;

			anMatIDs[iTriangle * 3 + iVertex] = 0;
		}
	}
	// Upload vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_POSITION]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nPositionsSiz, aflPositions, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_POSITION);
	// Upload vertex_normals
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_NORMAL]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nNormalsSiz, aflNormals, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_NORMAL);
	// Upload UVs
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_UV]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nUVsSiz, aflUVs, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_UV, 2, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_UV);
	// Upload bones
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_BONE]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nBoneIDsSiz, anBoneIDs, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_BONE, 1, GL_UNSIGNED_INT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_BONE);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] aflPositions;
	delete[] aflNormals;
	delete[] aflUVs;
	delete[] anBoneIDs;
	delete[] anMatIDs;

	return iVAO;
}

void renderer::model_load_loop()
{
	RESTRICT_THREAD_RENDERING;
	PRINT_DBG("renderer::model_load_loop entered");
	while (m_bLoading)
	{
		while (m_bLoading && (m_ldmdl_cmdbuf.IsEmpty()));
		if (!m_ldmdl_cmdbuf.IsEmpty())
		{
			ldmdl_cmd_t* pCommands;
			size_t nCommands = 0;
			m_ldmdl_cmdbuf.BeginRead(&pCommands, &nCommands);
			if (nCommands == 0)
			{
				continue;
			}
			PRINT_DBG("renderer: received " << nCommands << " model load request(s)");
			while (nCommands--)
			{
				PRINT_DBG("renderer: received model load request for " << pCommands->szFilename);
				mdlc::smd_parser parser(pCommands->szFilename);
				model mdl = parser.get_model();

				
				m_iLoadedModelID = upload_model(mdl);
				//PRINT_DBG("renderer: model uploaded!");
				while (m_iLoadedModelID) {
					Sleep(1);
				}
				//PRINT_DBG("renderer: model delivered!");
				
				pCommands++;
			}
			m_ldmdl_cmdbuf.EndRead();
		}
	}
	PRINT_DBG("renderer::model_load_loop ended");
}
