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

#include <gfx/etf.h>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

using namespace gfx;

gfx::gfx_global gGfx;
gfx::gfx_global* gpGfx = &gGfx;

static void gfx_debug_callback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	gfx::gfx_global* pGfx = (gfx::gfx_global*)userParam;
	if (type == GL_DEBUG_TYPE_ERROR) {
		PRINT_DBG("==============");
		PRINT_DBG("OpenGL Error:");
		PRINT_DBG(message);
		switch (source) {
		case GL_DEBUG_SOURCE_API: PRINT_DBG("Source: API"); break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: PRINT_DBG("Source: Window system"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: PRINT_DBG("Source: Shader compiler"); break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: PRINT_DBG("Source: Third party"); break;
		case GL_DEBUG_SOURCE_APPLICATION: PRINT_DBG("Source: Application"); break;
		case GL_DEBUG_SOURCE_OTHER: PRINT_DBG("Source: Other"); break;
		}
		PRINT_DBG("ID: " << id);
		PRINT_DBG("==============");
		ASSERT(0);
	} else if (type == GL_DEBUG_TYPE_PERFORMANCE) {
		PRINT_DBG("OpenGL Performance Warning:");
		PRINT_DBG(message);
	} else {
		//PRINT_DBG("OpenGL Warning:");
		//PRINT_DBG(message);
	}
}

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

#if defined(PLAT_DEBUG)
	if (glDebugMessageCallback) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(gfx_debug_callback, (void*)this);
	} else {
		PRINT_DBG("No glDebugMessageCallback!!");
	}
#endif

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

	SDL_GL_SetSwapInterval(1);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplSdlGL3_Init(pWindow);
	ImGui::StyleColorsClassic();

	io.Fonts->AddFontDefault();

	flCurrentTime = 0.0;

	if (TTF_Init() == 0) {
		pDebugFont = TTF_OpenFont("data/resources/debug.ttf", 14);
		if (!pDebugFont) {
			PRINT_ERR("Failed to load debug font: " << TTF_GetError());
#ifdef PLAT_WINDOWS
			char buf[MAX_PATH];
			GetCurrentDirectoryA(MAX_PATH, buf);
			PRINT_ERR("\tWIN32: current directory: " << buf);
#endif
		}
		ASSERT(pDebugFont);
	} else {
		PRINT_ERR("Failed to initialize SDL_TTF: " << TTF_GetError());
		pDebugFont = nullptr;
		ASSERT(0);
	}

	// Create and upload built-in quad
	float aflQuadPos[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	
	float aflQuadUV[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	glGenVertexArrays(1, &m_iModelQuadVAO);
	glBindVertexArray(m_iModelQuadVAO);
	glGenBuffers(1, &m_iModelQuadVBOPos);
	glGenBuffers(1, &m_iModelQuadVBOUV);

	glBindBuffer(GL_ARRAY_BUFFER, m_iModelQuadVBOPos);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), aflQuadPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_iModelQuadVBOUV);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), aflQuadUV, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	return true;
}

void gfx::gfx_global::init_glonly() {
	gladLoadGLLoader(SDL_GL_GetProcAddress);
}

bool gfx::gfx_global::shutdown()
{
	for (auto& shader : shaders)
		shader.reset();
	for (auto& kv : filename_texture_map)
		kv.second.reset();
	if (pDebugFont) {
		TTF_CloseFont(pDebugFont);
	}
	ImGui_ImplSdlGL3_Shutdown();
	ImGui::DestroyContext();
	if (pGLContext) {
		SDL_GL_DeleteContext(pGLContext);
		pGLContext = nullptr;
	}
	if (pRenderer) {
		SDL_DestroyRenderer(pRenderer);
		pRenderer = nullptr;
	}
	if (pWindow) {
		SDL_DestroyWindow(pWindow);
		pWindow = nullptr;
	}
	SDL_Quit();
	return true;
}

void gfx::gfx_global::begin_frame(bool bGUI)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(bGUI)
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

void gfx::gfx_global::end_frame(bool bGUI)
{
	if (bGUI) {
		ImGui::Render();
		ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
	}
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

	// We could allow zero size here, but that is nonsensical
	if (nViewportW > 0 && nViewportH > 0) {
		glViewport(nViewportX, nViewportY, nViewportW, nViewportH);
		glScissor(nViewportX, nViewportY, nViewportW, nViewportH);
		glEnable(GL_SCISSOR_TEST);
	}
}

void gfx::gfx_global::set_viewport(int w, int h) {
	if(w > 0 && h > 0)
		glViewport(0, 0, w, h);
}

void gfx::gfx_global::restore_viewport()
{
	GLsizei W = nWidth;
	GLsizei H = nHeight;
	glViewport(0, 0, W, H);
	glScissor(0, 0, W, H);
	glDisable(GL_SCISSOR_TEST);
}

void gfx::gfx_global::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gfx::gfx_global::clear_color(float r, float g, float b) {
	if (r < 0 || g < 0 || b < 0) {
		glClearColor(0.39215686274f, 0.58431372549f, 0.9294117647f, 1.0f);
	} else {
		glClearColor(r, g, b, 1.0f);
	}
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

size_t gfx::gfx_global::load_shader(gfx::shared_shader_program pShaderProgram)
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
	gfx::shared_shader_program pShaderProgram = std::make_shared<gfx::shader_program>(filename.c_str());
	PRINT_ERR("gfx: attempting to load shader " << filename);
	return load_shader(pShaderProgram);
}

gfx::shared_tex2d gfx::gfx_global::load_texture(const std::string& filename)
{
	// is the texture already loaded?
	if (filename_texture_map.count(filename)) {
		return filename_texture_map[filename];
	}

	gfx::texture_format iTexFmt = texfmt_rgb;
	void* pImageBuf;
	size_t nWidth;
	size_t nHeight;

	// Check filename extension
	size_t iExt = filename.rfind('.');
	bool bETF = false;
	if (iExt != std::string::npos) {
		auto sExt = filename.substr(iExt + 1, filename.length() - 1);
		std::transform(sExt.begin(), sExt.end(), sExt.begin(), ::tolower);
		bETF = (sExt == "etf");
	}

	if (bETF) {
		gfx::etf::etf_reader etfr(filename);
		
	} else {
		SDL_Surface* pSurf;
		
		pSurf = IMG_Load(filename.c_str());

		if (!pSurf) {
			PRINT_ERR("gfx: failed to load texture: " << filename);
			ASSERT_IMAGE(pSurf);
			return nullptr;
		}

		ASSERT(pSurf->format);

		if (pSurf->format->BytesPerPixel == 4)
			iTexFmt = texfmt_rgba;

		ASSERT(pSurf->pixels);

		gfx::shared_tex2d tex = std::make_shared<gfx::texture2d>();
		tex->bind();
		tex->wrap(texw_repeat);
		tex->filtering(texfilt_linear);
		tex->upload(pSurf->pixels, iTexFmt, pSurf->w, pSurf->h);
		tex->generate_mipmap();
		filename_texture_map.emplace(filename, tex);

		SDL_FreeSurface(pSurf);

		PRINT_DBG("gfx: loaded texture " << filename);

		return tex;
	}
}

gfx::model_id gfx::gfx_global::load_model(const std::string & filename)
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

gfx::model_id gfx::gfx_global::load_model(const gfx::model & mdl)
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
	std::unique_ptr<float[]> aflPositions = std::make_unique<float[]>(nPositionsSiz);
	std::unique_ptr<float[]> aflNormals = std::make_unique<float[]>(nNormalsSiz);
	std::unique_ptr<float[]> aflUVs = std::make_unique<float[]>(nUVsSiz);
	std::unique_ptr<uint32_t[]> anBoneIDs = std::make_unique<uint32_t[]>(nBoneIDsSiz);
	std::unique_ptr<uint8_t[]> anMatIDs = std::make_unique<uint8_t[]>(nMatIDsSiz);
	//float* aflPositions = new float[nPositionsSiz];
	//float* aflNormals = new float[nNormalsSiz];
	//float* aflUVs = new float[nUVsSiz];
	//uint32_t* anBoneIDs = new uint32_t[nBoneIDsSiz];
	//uint8_t* anMatIDs = new uint8_t[nMatIDsSiz * 3];

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
	glBufferData(GL_ARRAY_BUFFER, nPositionsSiz * sizeof(float), aflPositions.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_POSITION);
	// Upload vertex_normals
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_NORMAL]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nNormalsSiz * sizeof(float), aflNormals.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_NORMAL);
	// Upload UVs
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_UV]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nUVsSiz * sizeof(float), aflUVs.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_UV, 2, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_UV);
	// Upload bones
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_BONE]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nBoneIDsSiz * sizeof(uint32_t), anBoneIDs.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_BONE, 1, GL_UNSIGNED_INT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_BONE);
	// Upload material IDs
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_MAT]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nMatIDsSiz * sizeof(uint8_t), anMatIDs.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_MAT, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0); ASSERT_OPENGL();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//delete[] aflPositions;
	//delete[] aflNormals;
	//delete[] aflUVs;
	//delete[] anBoneIDs;
	//delete[] anMatIDs;

	model_vertexcount_map.emplace(iVAO, mdl.triangles.size() * 3);

	//gpGlobals->pStatistics->get_stat_u(ESTAT_C_RENDERER, "models_loaded") += 1;

	model_collider_map.emplace(iVAO, mdl.collider);

	gfx::model_id ret(iVAO);
	return ret;
}

size_t gfx::gfx_global::load_material(const std::string & filename)
{
	mdlc::qc qcp = mdlc::qc(std::ifstream(filename));

	material mat(qcp);

	auto iszShader = mat.get_shader_name();

	for (auto& pShader : shaders) {
		if (pShader) {
			if (iszShader == pShader->get_name()) {
				//pShader->load_material(mat);
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
	if (mdl) {
		glBindVertexArray(mdl);
		current_model = mdl;
	} else {
		ASSERT(0);
	}
}

void gfx::gfx_global::unbind_model()
{
	glBindVertexArray(0);
	current_model = 0;
}

void gfx::gfx_global::draw_model()
{
	if (current_model) {
		if (model_vertexcount_map.count(current_model)) {
			glDrawArrays(GL_TRIANGLES, 0, model_vertexcount_map[current_model]);
		} else {
			ASSERT(0);
		}
	} else {
		ASSERT(0);
	}
}
/*
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
*/
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

model_id gfx::gfx_global::upload_terrain(const elf::terrain_chunk & chunk) {
	GLuint iVAO;
	// Used buffers:
	// MDL_VBO_POSITION, MDL_VBO_NORMAL, MDL_VBO_UV, MDL_VBO_MAT
	GLuint aiVBO[MDL_VBO_MAX];

	glGenVertexArrays(1, &iVAO); ASSERT_OPENGL();
	glBindVertexArray(iVAO); ASSERT_OPENGL();
	glGenBuffers(MDL_VBO_MAX, aiVBO); ASSERT_OPENGL();

	// Every tile is made out of two triangles; T = 2
	// Every triangle is made out of 3 points; P = 3
	// Those three points are made out of 3 floats; V = 3
	// ((W-1) * (H-1)) * T * P * V * sizeof(float)
	size_t nTriangles = ((chunk.width - 1) * (chunk.height - 1)) * 2;
	size_t nSizPosition = nTriangles * 3 * 3 * sizeof(float);
	size_t nSizNormal = nTriangles * 3 * 3 * sizeof(float);
	size_t nSizUV = nTriangles * 3 * 2 * sizeof(float);
	size_t nSizTex = nTriangles * sizeof(GLubyte);

	std::unique_ptr<float[]> pBufPosition = std::make_unique<float[]>(nSizPosition);
	std::unique_ptr<float[]> pBufNormal = std::make_unique<float[]>(nSizNormal);
	std::unique_ptr<float[]> pBufUV = std::make_unique<float[]>(nSizUV);
	std::unique_ptr<GLubyte[]> pBufTex = std::make_unique<GLubyte[]>(nSizTex);

	for (int y = 0; y < chunk.height - 1; y++) {
		for (int x = 0; x < chunk.width - 1; x++) {
			size_t iBase = y * chunk.width + x;
			
			vec3 vtl(x + 0.f, y + 0.f, chunk.heightmap[iBase + 0]);
			vec3 vtr(x + 1.f, y + 0.f, chunk.heightmap[iBase + 1]);
			vec3 vbl(x + 0.f, y + 1.f, chunk.heightmap[iBase + chunk.width + 0]);
			vec3 vbr(x + 1.f, y + 0.f, chunk.heightmap[iBase + chunk.width + 1]);

			vec3 vertices1[3] = {
				{ x + 0.f, y + 0.f, chunk.heightmap[iBase + 0] }, // TL
				{ x + 0.f, y + 1.f, chunk.heightmap[iBase + chunk.width + 0] }, // BL
				{ x + 1.f, y + 0.f, chunk.heightmap[iBase + chunk.width + 1] }, // BR
			};
			vec3 vertices2[3] = {
				{ x + 0.f, y + 0.f, chunk.heightmap[iBase + 0] }, // TL
				{ x + 1.f, y + 0.f, chunk.heightmap[iBase + chunk.width + 1] }, // BR
				{ x + 1.f, y + 0.f, chunk.heightmap[iBase + 1] }, // TR
			};

			// Triangle 1

			size_t iTriangleBase_V3 = (y * chunk.width + x) * (2 * (3 * 3 * sizeof(float)));
			size_t iTriangleBase_V2 = (y * chunk.width + x) * (2 * (3 * 2 * sizeof(float)));
			size_t iTriangleBase_V1u = (y * chunk.width + x) * (2 * (3 * sizeof(GLubyte)));

			for (int v = 0; v < 3; v++) {
				pBufPosition[iTriangleBase_V3 + v * 3 + 0] = vertices1[v].x();
				pBufPosition[iTriangleBase_V3 + v * 3 + 1] = vertices1[v].y();
				pBufPosition[iTriangleBase_V3 + v * 3 + 2] = vertices1[v].z();

				pBufNormal[iTriangleBase_V3 + v * 3 + 0] = 0;
				pBufNormal[iTriangleBase_V3 + v * 3 + 1] = 1;
				pBufNormal[iTriangleBase_V3 + v * 3 + 2] = 0;

				pBufUV[iTriangleBase_V2 + v * 2 + 0] = 0;
				pBufUV[iTriangleBase_V2 + v * 2 + 1] = 0;

				pBufTex[iTriangleBase_V1u + v] = 0;
			}
		}
	}

	// Upload vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_POSITION]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nSizPosition, pBufPosition.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_POSITION);
	// Upload vertex_normals
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_NORMAL]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nSizNormal, pBufNormal.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_NORMAL);
	// Upload UVs
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_UV]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nSizUV, pBufUV.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_UV, 2, GL_FLOAT, GL_FALSE, 0, 0); ASSERT_OPENGL();
	glEnableVertexAttribArray(MDL_VBO_UV);
	// Upload texture IDs
	glBindBuffer(GL_ARRAY_BUFFER, aiVBO[MDL_VBO_MAT]); ASSERT_OPENGL();
	glBufferData(GL_ARRAY_BUFFER, nSizTex, pBufTex.get(), GL_STATIC_DRAW); ASSERT_OPENGL();
	glVertexAttribPointer(MDL_VBO_MAT, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0); ASSERT_OPENGL();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void gfx::gfx_global::draw_terrain(const model_id& id) {
}

void gfx::gfx_global::draw_framebuffer(gfx::shared_fb& fb) {
	gfx::shared_shader_program pShader = shaders[get_shader_program_index("delivery_fb")];
	ASSERT(pShader);
	if (!pShader)
		return;
	pShader->reload();
	pShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fb->diffuse()->handle());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fb->normal()->handle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, fb->worldpos()->handle());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, fb->specular()->handle());
	pShader->set_int("tex_diffuse", 0);
	pShader->set_int("tex_normal", 1);
	pShader->set_int("tex_worldpos", 2);
	pShader->set_int("tex_specular", 3);
	
	glBindVertexArray(m_iModelQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void gfx::gfx_global::depth_test(bool bEnable) {
	if (bEnable) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void gfx::gfx_global::wireframe(bool bEnable) {
	if(bEnable)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void gfx::gfx_global::blend(bool bEnable) {
	if (bEnable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

gfx::debug_marker::debug_marker(const std::string & str) {
#if defined(PLAT_DEBUG)
	if (glad_glPushDebugGroup) {
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 10, str.size() + 1, str.c_str());
	} else if (glad_glPushGroupMarkerEXT) {
		glPushGroupMarkerEXT(str.size() + 1, str.c_str());
	} 
#endif
}

gfx::debug_marker::~debug_marker() {
#if defined(PLAT_DEBUG)
	if (glad_glPopDebugGroup) {
		glPopDebugGroup();
	} else if (glad_glPopGroupMarkerEXT) {
		glPopGroupMarkerEXT();
	}
#endif
}
