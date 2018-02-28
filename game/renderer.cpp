#include "stdafx.h"
#include "renderer.h"

#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <GL/glu.h>
#include <assert_opengl.h>

#include "qc_parser.h"
#include "smd_parser.h"
#include "shader_program.h"
#include "event_handler.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gui/imgui_impl_sdl_gl3.h"

static void opengl_msg_callback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);

void renderer::open_window(const char * szTitle, int nWidth, int nHeight, bool bFullscreen)
{
	RESTRICT_THREAD_RENDERING;
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	m_pWindow = SDL_CreateWindow(szTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, nWidth, nHeight, (bFullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	ASSERT_SDL2(m_pWindow);
	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	ASSERT_SDL2(m_pRenderer);

	m_matProj = glm::perspective(glm::radians(90.f), (float)nWidth / (float)nHeight, 0.1f, 1000.f);
}

void renderer::close_window()
{
	RESTRICT_THREAD_RENDERING;
	if (m_pRenderer)
		SDL_DestroyRenderer(m_pRenderer);
	if (m_pWindow)
		SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}

void renderer::render()
{
	RESTRICT_THREAD_RENDERING;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_vecPrograms.size() == 0)
		return;
	
	ImGui_ImplSdlGL3_NewFrame(m_pWindow);

	// Draw models
	drawcmd_t* pCommands;
	size_t nCommands = 0;
	size_t nAllVertices = 0;
	while (m_cmdbuf.is_empty());
	if(m_cmdbuf.begin_read(&pCommands, &nCommands))
	{
		//PRINT_DBG("renderer::render: drawing " << nCommands << " models");
		shader_program* pShader = m_vecPrograms[0];
		glUseProgram(pShader->get_id());
		gpGlobals->pStatistics->get_stat_u(ESTAT_C_RENDERER, "current draw commands") = nCommands;
		while (nCommands--)
		{
			//PRINT_DBG("renderer::render: drawing model #" << pCommands->iModelID);
			glBindVertexArray(pCommands->iModelID); ASSERT_OPENGL();
			
			glm::mat4 mat_trans(1.0);
			mat_trans = glm::scale(mat_trans, glm::vec3(pCommands->flScale, pCommands->flScale, pCommands->flScale));
			mat_trans = pCommands->matRotation * mat_trans;
			mat_trans = glm::translate(mat_trans, glm::vec3(pCommands->vecPosition[0], pCommands->vecPosition[1], pCommands->vecPosition[2]));
			
			pShader->set_mat_trans(glm::value_ptr(mat_trans));

			// activate textures
			for (size_t i = 0; i < m_mapTextures[pCommands->iModelID].size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); ASSERT_OPENGL();
				glBindTexture(GL_TEXTURE_2D, m_mapTextures[pCommands->iModelID][i]); ASSERT_OPENGL();
			}

			size_t nVertexCount = m_model_vertexcount[pCommands->iModelID];
			nAllVertices += nVertexCount;

			glDrawArrays(GL_TRIANGLES, 0, nVertexCount); ASSERT_OPENGL();

			pCommands++;
		}

		m_cmdbuf.end_read();
	}

	gpGlobals->pStatistics->get_stat_u(ESTAT_C_RENDERER, "Vertex count") = nAllVertices;

	m_flNow = gpGlobals->curtime;
	if (m_flNow - m_flLast > 1)
	{
		m_flFPS = (float)m_nFrames / (m_flNow - m_flLast);
		m_flLast = m_flNow;
		m_nFrames = 0;
		char szNewWindowTitle[64];
		snprintf(szNewWindowTitle, 64, "engine - %d FPS", (int)ceilf(m_flFPS));
		SDL_SetWindowTitle(m_pWindow, szNewWindowTitle);
	}

	draw_debug_tools();
	ImGui::Render();
	ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());

	m_nFrames++;
	SDL_GL_SwapWindow(m_pWindow);

	// Pass SDL events to the event handler on the logic thread
	std::vector<event_t> vecEvents;
	event_t event;
	while (SDL_PollEvent(&event.event))
	{
		event.nTime = (uint64_t)(gpGlobals->curtime * 1000);
		ImGui_ImplSdlGL3_ProcessEvent(&event.event);
		vecEvents.push_back(event);
	}
	if(vecEvents.size() > 0)
		gpGlobals->pEventHandler->push_event(vecEvents);
}

bool renderer::init_gl()
{
	RESTRICT_THREAD_RENDERING;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
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

	SDL_GL_SetSwapInterval(-1);

	int nWidth, nHeight;
	SDL_GetWindowSize(m_pWindow, &nWidth, &nHeight);

	glViewport(0, 0, nWidth, nHeight);
	ASSERT_OPENGL();
	glClearColor(0.39215686274f, 0.58431372549f, 0.9294117647f, 1.0f);
	ASSERT_OPENGL();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_MULTISAMPLE);

#ifdef PLAT_DEBUG
	//glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback((GLDEBUGPROC)opengl_msg_callback, 0);
#endif

	return true;
}

void renderer::shutdown_gl()
{
	RESTRICT_THREAD_RENDERING;

	for (auto pProgram : m_vecPrograms)
	{
		delete pProgram;
	}

	if (m_pGLContext)
		SDL_GL_DeleteContext(m_pGLContext);
}


model_id renderer::load_model(const char * szFilename)
{
	RESTRICT_THREAD_LOGIC;

	std::string iszFilename(szFilename);
	auto it = m_mapModels.find(iszFilename);
	if (it != m_mapModels.end())
	{
		PRINT_DBG("Model " << szFilename << " already found");
		return (*it).second;
	}
	if (!m_bLoading)
	{
		PRINT_ERR("Late load_model");
		ASSERT(m_bLoading);
	}

	/*if (m_iLoadedModelID)
	{
		PRINT_DBG("renderer::load_model: waiting to upload...");
	}*/
	while (m_iLoadedModelID) {
		Sleep(1);
	}

	gfx_load_cmd_t c;
	c.type = GFX_LD_T_MDL;
	strncpy(c.szFilename, szFilename, GFX_LD_CMD_MAX_FN);
	m_gfx_ld_cmdbuf.begin_write();
	m_gfx_ld_cmdbuf.write(c);
	m_gfx_ld_cmdbuf.end_write();
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
	//m_mapModels.emplace(iszFilename, ret);
	return ret;
}

void renderer::load_models(std::vector<std::string> filenames)
{
	RESTRICT_THREAD_LOGIC;
	while (m_iLoadedModelID) {
		Sleep(1);
	}

	gfx_load_cmd_t c;
	c.type = GFX_LD_T_MDL;
	//model_ids.clear();
	m_gfx_ld_cmdbuf.begin_write();
	for (auto& fn : filenames)
	{
		// check if already loaded
		auto it = m_mapModels.find(fn);
		if (it != m_mapModels.end())
		{
			PRINT_DBG("Model " << fn << " already found");
			//model_ids.push_back((*it).second);
			continue;
		}

		strncpy(c.szFilename, fn.c_str(), GFX_LD_CMD_MAX_FN);
		m_gfx_ld_cmdbuf.write(c);
	}
	m_gfx_ld_cmdbuf.end_write();
}

void renderer::draw_model(size_t iModelID, vec & vecPosition, float flRotation)
{
	RESTRICT_THREAD_LOGIC;
	//drawcmd_t c = { iModelID, vecPosition, flRotation };
	//m_cmdbuf.begin_write();
	//m_cmdbuf.write(c);
	//m_cmdbuf.end_write();
}

void renderer::draw_models(std::vector<drawcmd_t>& cmds)
{
	RESTRICT_THREAD_LOGIC;
	m_cmdbuf.begin_write();
	for (auto& cmd : cmds)
		m_cmdbuf.write(cmd);
	m_cmdbuf.end_write();
}

void renderer::load_shader(const char * szFilename)
{
	RESTRICT_THREAD_LOGIC;
	gfx_load_cmd_t c;
	c.type = GFX_LD_T_SHADER;
	PRINT_DBG("renderer::load_shader: requesting load of shader: " << szFilename);
	strncpy(c.szFilename, szFilename, GFX_LD_CMD_MAX_FN);
	m_gfx_ld_cmdbuf.begin_write();
	m_gfx_ld_cmdbuf.write(c);
	m_gfx_ld_cmdbuf.end_write();
}

model_id renderer::upload_model(const model& mdl)
{
	RESTRICT_THREAD_RENDERING;
	GLuint iVAO;
	GLuint aiVBO[MDL_VBO_MAX];

	glGenVertexArrays(1, &iVAO); ASSERT_OPENGL();
	glBindVertexArray(iVAO); ASSERT_OPENGL();
	glGenBuffers(MDL_VBO_MAX, aiVBO); ASSERT_OPENGL();

	// Load textures
	std::vector<uint32_t> aTextures(mdl.materials.size(), 0);
	for (size_t i = 0; i < mdl.materials.size(); i++)
	{
		uint32_t iTex = load_texture(mdl.materials[i].szName);
		aTextures[i] = iTex;
	}
	m_mapTextures.emplace(iVAO, aTextures);

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
	uint8_t* anMatIDs = new uint8_t[nMatIDsSiz * 3];

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
			anMatIDs[iTriangle * 3 + iVertex] = mdl.triangles[iTriangle].iModelMaterial;
		}
	}
	// Upload vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_POSITION]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nPositionsSiz * sizeof(float), aflPositions, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_POSITION);
	// Upload vertex_normals
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_NORMAL]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nNormalsSiz * sizeof(float), aflNormals, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_NORMAL);
	// Upload UVs
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_UV]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nUVsSiz * sizeof(float), aflUVs, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_UV, 2, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_UV);
	// Upload bones
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_BONE]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nBoneIDsSiz * sizeof(uint32_t), anBoneIDs, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_BONE, 1, GL_UNSIGNED_INT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_BONE);
	// Upload material IDs
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_MAT]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nMatIDsSiz * sizeof(uint8_t), anMatIDs, GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_MAT, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0); ASSERT_OPENGL();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] aflPositions;
	delete[] aflNormals;
	delete[] aflUVs;
	delete[] anBoneIDs;
	delete[] anMatIDs;

	m_model_vertexcount.emplace((model_id)iVAO, mdl.triangles.size() * 3);

	gpGlobals->pStatistics->get_stat_u(ESTAT_C_RENDERER, "models_loaded") += 1;

	return iVAO;
}

uint32_t renderer::load_texture(const std::string & filename)
{
	SDL_Surface* pSurf;
	uint32_t iTex;
	int iTexFmt = GL_RGB;

	// is the texture already loaded?
	if (m_map_texture_name.count(filename))
	{
		return m_map_texture_name[filename];
	}

	pSurf = IMG_Load(filename.c_str());

	PRINT_DBG("renderer::load_texture: loading " << filename);

	if (!pSurf) {
		PRINT_ERR("Failed to load texture: " << filename << " (assertion incoming)");
		ASSERT_IMAGE(pSurf);
		return 0;
	}

	ASSERT(pSurf->format);

	if (pSurf->format->BytesPerPixel == 4)
		iTexFmt = GL_RGBA;

	glGenTextures(1, &iTex); ASSERT_OPENGL();
	glBindTexture(GL_TEXTURE_2D, iTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ASSERT(pSurf->pixels);

	glTexImage2D(GL_TEXTURE_2D, 0, iTexFmt, pSurf->w, pSurf->h, 0, iTexFmt, GL_UNSIGNED_BYTE, pSurf->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	m_map_texture_name.emplace(filename, iTex);

	SDL_FreeSurface(pSurf);

	return iTex;
}

void renderer::load_loop()
{
	RESTRICT_THREAD_RENDERING;
	PRINT_DBG("renderer::model_load_loop entered");
	while (m_bLoading)
	{

		gfx_load_cmd_t* pCommands;
		size_t nCommands = 0;
		if (!m_gfx_ld_cmdbuf.begin_read(&pCommands, &nCommands))
			continue;
		//PRINT_DBG("renderer: received " << nCommands << " load request(s)");
		while (nCommands--)
		{
			model mdl;
			mdlc::smd_parser parser;
			switch (pCommands->type)
			{
			case GFX_LD_T_MDL:
				PRINT_DBG("renderer: received model load request for " << pCommands->szFilename);
				parser = mdlc::smd_parser(pCommands->szFilename);
				mdl = parser.get_model();


				m_iLoadedModelID = upload_model(mdl);
				m_mapModels.emplace(std::string(pCommands->szFilename), m_iLoadedModelID);
				PRINT_DBG("renderer: model uploaded!");
				while (m_iLoadedModelID) {
					Sleep(1);
				}
				PRINT_DBG("renderer: model delivered!");
				break;
			case GFX_LD_T_SHADER:
				shader_program* pProgram = new shader_program(pCommands->szFilename);
				PRINT_DBG("renderer: received shader load request for " << pProgram->get_name());
				m_vecPrograms.push_back(pProgram);
				pProgram->set_mat_proj(glm::value_ptr(m_matProj));
				break;
			}
				
				
			pCommands++;
		}
		m_gfx_ld_cmdbuf.end_read();
	}
	PRINT_DBG("renderer::model_load_loop ended");
}

void renderer::update_camera(glm::vec3& pos, glm::mat4& rot)
{
	RESTRICT_THREAD_RENDERING;
	if (m_vecPrograms.size() < 1)
		return;
	//m_matView = rot.operator glm::mat<4, 4, float, glm::packed_highp>();
	m_matView = rot;
	m_matView = glm::translate(m_matView, glm::vec3(pos));

	shader_program* pShader = m_vecPrograms[0];
	pShader->set_mat_view(glm::value_ptr(m_matView));
}

void renderer::init_gui()
{
	PRINT_DBG("renderer::init_gui: initializing imgui");
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplSdlGL3_Init(m_pWindow);
	ImGui::StyleColorsClassic();

	io.Fonts->AddFontDefault();
}

void renderer::shutdown_gui()
{
	PRINT_DBG("renderer::shutdown_gui: shutting down");
	ImGui_ImplSdlGL3_Shutdown();
	ImGui::DestroyContext();
}

static void opengl_msg_callback(GLenum iSource, GLenum iType, GLuint iID, GLenum iSeverity, GLsizei nLength, const GLchar* szMsg, const void* pUParam)
{
	fprintf(stderr, "[ OpenGL Message ] type = 0x%x, severity = 0x%x, message = %s\n",
		iType, iSeverity, szMsg);
}
