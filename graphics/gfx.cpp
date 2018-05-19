#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/shader_program.h>
#include "glad/glad.h"
#include "gui/imgui.h"
#include "gui/imgui_impl_sdl_gl3.h"
#include <assert_opengl.h>
#include <gfx/model.h>

#include <gfx/smd_loader.h>
#include <gfx/emf_loader.h>

#include <SDL2/SDL_image.h>

using namespace gfx;

gfx::gfx_global gGfx;
gfx::gfx_global* gpGfx = &gGfx;

bool gfx::gfx_global::init(const char* szTitle, size_t width, size_t height, size_t glVersionMajor, size_t glVersionMinor)
{
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	pWindow = SDL_CreateWindow(szTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!pWindow) return false;
	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!pRenderer) return false;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glVersionMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glVersionMinor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	pGLContext = SDL_GL_CreateContext(pWindow);

	if (!pGLContext)
	{
		ASSERT_SDL2(0);
		return false;
	}

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	SDL_GetWindowSize(pWindow, &nWidth, &nHeight);

	glViewport(0, 0, nWidth, nHeight);
	glClearColor(0.39215686274f, 0.58431372549f, 0.9294117647f, 1.0f);

	glEnable(GL_BLEND); ASSERT_OPENGL();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); ASSERT_OPENGL();

	glEnable(GL_CULL_FACE); ASSERT_OPENGL();
	glCullFace(GL_BACK); ASSERT_OPENGL();
	glFrontFace(GL_CCW); ASSERT_OPENGL();

	glEnable(GL_DEPTH_TEST); ASSERT_OPENGL();
	glDepthFunc(GL_LEQUAL); ASSERT_OPENGL();

	glEnable(GL_MULTISAMPLE); ASSERT_OPENGL();

	SDL_GL_SetSwapInterval(-1);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplSdlGL3_Init(pWindow);
	ImGui::StyleColorsClassic();

	io.Fonts->AddFontDefault();

	flCurrentTime = 0.0;

	return true;
}

bool gfx::gfx_global::shutdown()
{
	ImGui_ImplSdlGL3_Shutdown();
	ImGui::DestroyContext();
	if (pGLContext)
		SDL_GL_DeleteContext(pGLContext);
	if (pRenderer)
		SDL_DestroyRenderer(pRenderer);
	if (pWindow)
		SDL_DestroyWindow(pWindow);
	SDL_Quit();
	return true;
}

void gfx::gfx_global::begin_frame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplSdlGL3_NewFrame(pWindow);
	
	nTimePrev = nTimeNow;
	nTimeNow = SDL_GetPerformanceCounter();

	flDeltaTime = ((nTimeNow - nTimePrev) * 1000.0f / (double)SDL_GetPerformanceFrequency()) * 0.001;
	flCurrentTime += flDeltaTime;
}

void gfx::gfx_global::draw_windows()
{
	if (nViewportX != -1)
		glViewport(0, 0, nWidth, nHeight);
	for (auto pWindow : windows)
		if(pWindow)
			pWindow->draw_window();
	if (nViewportX != -1) {
		glViewport(nViewportX, nViewportY, nViewportW, nViewportH);
	}
}

void gfx::gfx_global::end_frame()
{
	ImGui::Render();
	ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(pWindow);
	if (m_event_handler) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			m_event_handler(e);
		}
	}
}

bool gfx::gfx_global::handle_events()
{
	SDL_Event event;

	if (m_event_handler)
		return false;
	
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.type == SDL_WINDOWEVENT_CLOSE) {
				return true;
			}
		}
		ImGui_ImplSdlGL3_ProcessEvent(&event);
	}
	return false;
}

void gfx::gfx_global::set_viewport(int sx, int sy, int ex, int ey)
{
	// Transform [relative to upper-left corner] coordinates to [relative to lower-left corner]
	nViewportX = sx;
	nViewportY = nHeight - ey;
	nViewportW = ex - sx;
	nViewportH = ey - sy;
	glViewport(nViewportX, nViewportY, nViewportW, nViewportH); ASSERT_OPENGL();
}

void gfx::gfx_global::restore_viewport()
{
	glViewport(0, 0, nWidth, nHeight); ASSERT_OPENGL();
}

int gfx_global::get_shader_program_index(const std::string & name)
{
	if (shader_name_map.count(name)) {
		return shader_name_map.at(name);
	}

	PRINT_ERR("gfx: material requested unknown shader \"" << name << "\"!");
	ASSERT(0);
	return -1;
}

size_t gfx::gfx_global::load_shader(shader_program * pShaderProgram)
{
	if (pShaderProgram) {
		PRINT_DBG("gfx: loaded shader " << pShaderProgram->get_name());
		shaders.push_back(pShaderProgram);
		shader_name_map.emplace(pShaderProgram->get_name(), shaders.size() - 1);
		return shaders.size() - 1;
	}
	ASSERT(0);
	abort();
}

size_t gfx::gfx_global::load_shader(const std::string & filename)
{
	shader_program* pShaderProgram = new shader_program(filename.c_str());
	PRINT_ERR("gfx: attempting to load shader " << filename);
	return load_shader(pShaderProgram);
}

uint32_t gfx::gfx_global::load_texture(const std::string & filename)
{
	SDL_Surface* pSurf;
	uint32_t iTex;
	int iTexFmt = GL_RGB;

	// is the texture already loaded?
	if (filename_texture_map.count(filename))
	{
		return filename_texture_map[filename];
	}

	pSurf = IMG_Load(filename.c_str());

	PRINT_DBG("gfx::load_texture: loading " << filename);

	if (!pSurf) {
		PRINT_ERR("gfx: failed to load texture: " << filename);
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
	filename_texture_map.emplace(filename, iTex);

	SDL_FreeSurface(pSurf);

	return iTex;
}

uint32_t gfx::gfx_global::load_model(const std::string & filename)
{
	gfx::model mdl;
	gfx::smd_loader parser_smd;
	gfx::emf_loader parser_emf;
	const std::string emf_ext("emf");

	if (filename_model_map.count(filename)) {
		return filename_model_map[filename];
	}

	if (std::equal(emf_ext.rbegin(), emf_ext.rend(), filename.rbegin())) {
		parser_emf = gfx::emf_loader(filename);
		mdl = parser_emf.get_model();
	}
	else {
		PRINT_ERR("gfx(load_model): !!! using studiomdl files as models is OBSOLETED");
		parser_smd = gfx::smd_loader(filename.c_str());
		mdl = parser_smd.get_model();
	}
	auto mid = load_model(mdl);
	filename_model_map.emplace(filename, mid);
	return mid;
}

uint32_t gfx::gfx_global::load_model(const gfx::model & mdl)
{
	GLuint iVAO;
	GLuint aiVBO[MDL_VBO_MAX];

	glGenVertexArrays(1, &iVAO); ASSERT_OPENGL();
	glBindVertexArray(iVAO); ASSERT_OPENGL();
	glGenBuffers(MDL_VBO_MAX, aiVBO); ASSERT_OPENGL();

	// Load material
	//std::vector<uint32_t> aTextures(mdl.materials.size(), 0);
	//for (size_t i = 0; i < mdl.materials.size(); i++)
	//{
	//uint32_t iTex = load_texture(mdl.materials[i].szName);
	//aTextures[i] = iTex;
	//}
	//m_mapTextures.emplace(iVAO, aTextures);

	//mdl.materials[0].iModelMaterial

	//if (mdl.materials.size() == 0) {
	//	PRINT_DBG("Model has no materials!");
	//}

	size_t iMaterial = load_material(mdl.material);
	model_material_map.emplace(iVAO, iMaterial);

	PRINT_DBG("renderer::upload_model: material " << mdl.material << " (#" << iMaterial << ") assigned to model " << iVAO);

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
			aflPositions[iTriangle * 9 + iVertex * 3 + 0] = mdl.triangles[iTriangle][iVertex].pos.x();
			aflPositions[iTriangle * 9 + iVertex * 3 + 1] = mdl.triangles[iTriangle][iVertex].pos.y();
			aflPositions[iTriangle * 9 + iVertex * 3 + 2] = mdl.triangles[iTriangle][iVertex].pos.z();

			aflNormals[iTriangle * 9 + iVertex * 3 + 0] = mdl.triangles[iTriangle][iVertex].normal.x();
			aflNormals[iTriangle * 9 + iVertex * 3 + 1] = mdl.triangles[iTriangle][iVertex].normal.y();
			aflNormals[iTriangle * 9 + iVertex * 3 + 2] = mdl.triangles[iTriangle][iVertex].normal.z();

			aflUVs[iTriangle * 6 + iVertex * 2 + 0] = mdl.triangles[iTriangle][iVertex].u;
			aflUVs[iTriangle * 6 + iVertex * 2 + 1] = 1 - mdl.triangles[iTriangle][iVertex].v;

			anBoneIDs[iTriangle * 3 + iVertex] = mdl.triangles[iTriangle][iVertex].iBoneID;
			anMatIDs[iTriangle * 3 + iVertex] = 0;
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

	model_vertexcount_map.emplace((model_id)iVAO, mdl.triangles.size() * 3);

	//gpGlobals->pStatistics->get_stat_u(ESTAT_C_RENDERER, "models_loaded") += 1;

	model_collider_map.emplace((model_id)iVAO, mdl.collider);

	return iVAO;
}

size_t gfx::gfx_global::load_material(const std::string & filename)
{
	mdlc::qc qcp = mdlc::qc(std::ifstream(filename));

	material mat(qcp);

	auto iszShader = mat.get_shader_name();

	for (auto& pShader : shaders) {
		if (pShader) {
			if (iszShader == pShader->get_name()) {
				pShader->load_material(mat);
				break;
			}
		}
	}

	materials.push_back(mat);
	size_t iRet = materials.size() - 1;
	filename_material_map.emplace(filename, iRet);
	material_filename_map.emplace(iRet, filename);

	return iRet;
}

void gfx::gfx_global::unload_model(model_id mdl)
{

}

void gfx::gfx_global::bind_model(model_id mdl)
{
	glBindVertexArray(mdl); ASSERT_OPENGL();
	current_model = mdl;
}

void gfx::gfx_global::unbind_model()
{
	glBindVertexArray(0); ASSERT_OPENGL();
	current_model = 0;
}

void gfx::gfx_global::draw_model()
{
	glDrawArrays(GL_TRIANGLES, 0, model_vertexcount_map[current_model]); ASSERT_OPENGL();
}

long long int gfx::gfx_global::use_shader(long long int shader)
{
	if (shader == -1) {
		if (current_model != 0) {
			auto shaderid = materials[model_material_map[current_model]].get_shader();
			use_shader(shaderid);
			return shaderid;
		}
		return -1;
	}
	glUseProgram(shader); ASSERT_OPENGL();
	return shader;
}

void gfx::gfx_global::remove_window(std::shared_ptr<gfx::window> w)
{
	for (auto& pWnd : windows) {
		if (pWnd == w) {
			pWnd = nullptr;
			return;
		}
	}
}

bool gfx::gfx_global::gui_keyboard_focus() const
{
	return ImGui_ImpSdlGL3_KeyboardFocused();
}

void gfx::gfx_global::get_events(std::vector<SDL_Event>& v)
{
	SDL_Event event;

	if (m_event_handler)
		return;

	while (SDL_PollEvent(&event))
	{
		v.push_back(event);
	}
}

void gfx::gfx_global::gui_send_event(const SDL_Event & e)
{
	SDL_Event ec = e;
	ImGui_ImplSdlGL3_ProcessEvent(&ec);
}

void gfx::gfx_global::capture_mouse(bool b)
{
	SDL_SetRelativeMouseMode(b ? SDL_TRUE : SDL_FALSE);
}

void gfx::gfx_global::load_default_shaders() {
	std::ifstream file("data/shaders/shader_manifest.txt");
	if (!file)
		return;
	for (std::string line; std::getline(file, line);) {
		if(line.size() > 1 && line[0] != '/' && line[1] != '/') // skip comment lines
			load_shader(line);
	}
}
