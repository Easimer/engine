#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>

#include <SDL2/SDL.h>
#include <gfx/window.h>
#include <gfx/model.h>
#include <gfx/material.h>

namespace gfx {
	class shader;
	class shader_program;
	class material;

	typedef uint32_t model_id;

	enum mdl_vertexattrib {
		MDL_VBO_POSITION = 0, // contains vertex position (3 GLfloats)
		MDL_VBO_NORMAL = 1, // contains vertex normal (3 GLfloats)
		MDL_VBO_UV = 2, // contains vertex uv (2 GLfloats)
		MDL_VBO_BONE = 3, // contains bone ID (1 UNSIGNED_INT)
		MDL_VBO_MAT = 4, // contains material IDs
		MDL_VBO_MAX = 5
	};

	struct gfx_global {
	public:
		bool init(const char* szTitle = "engine", size_t width = 1280, size_t height = 720, size_t glVersionMajor = 3, size_t glVersionMinor = 3);
		bool shutdown();

		void begin_frame();
		void draw_windows();
		void end_frame();
		bool handle_events();

		// Start and end coordinates relative to the UPPER-LEFT CORNER of the screen
		void set_viewport(int sx, int sy, int ex, int ey);
		void restore_viewport();

		int get_shader_program_index(const std::string& name);

		size_t load_shader(const std::string& filename);
		uint32_t load_texture(const std::string& filename);
		uint32_t load_model(const std::string& filename);
		uint32_t load_model(const gfx::model& mdl);
		size_t load_material(const std::string& filename);

		void unload_model(model_id mdl);

		void bind_model(model_id mdl);
		void unbind_model();

		void draw_model();

		// value of -1 activates the shader assigned to the material
		// of the currently bound model
		long long int use_shader(long long int shader = -1);

		void add_window(gfx::window* w) { windows.push_back(w); }

		shader_program* get_shader(size_t i) { return shaders[i]; }

		float& delta() { return flDeltaTime; }
		float& curtime() { return flCurrentTime; }

		int width() const { return nWidth; }
		int height() const { return nHeight; }

		size_t model_vertices(model_id mdl) const { return model_vertexcount_map.at(mdl); }
		bool model_has_collider(model_id mdl) const { return model_collider_map.count(mdl) && model_collider_map.at(mdl).size() > 0; }
		const gfx::material& model_material(model_id mdl) const { return materials[model_material_map.at(mdl)]; }
		const std::string& model_material_filename(model_id mdl) const { return material_filename_map.at(model_material_map.at(mdl)); }

	private:
		SDL_Window* pWindow;
		SDL_GLContext pGLContext;
		SDL_Renderer* pRenderer;

		model_id current_model;

		float flDeltaTime = 0;
		float flCurrentTime = 0;
		float flLastTime = 0;

		int nWidth, nHeight;

		int nViewportX = -1, nViewportY = -1, nViewportW = -1, nViewportH = -1;

		std::vector<shader_program*> shaders;
		std::map<std::string, size_t> shader_name_map;

		std::vector<gfx::window*> windows;

		std::map<model_id, size_t> model_material_map;
		std::map<model_id, size_t> model_vertexcount_map;

		std::map<std::string, model_id> filename_model_map;
		std::map<model_id, std::vector<triangle>> model_collider_map;

		std::map<std::string, uint32_t> filename_texture_map;

		std::vector<gfx::material> materials;
		std::map<std::string, size_t> filename_material_map;
		std::map<size_t, std::string> material_filename_map;
	};
}

extern gfx::gfx_global* gpGfx;
