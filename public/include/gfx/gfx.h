#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <gfx/window.h>
#include <gfx/model.h>
#include <gfx/material.h>

#include <elf/elf.h>

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

		// Begin frame
		// Call this before any draw command
		void begin_frame();
		// Draw windows accounted by the window manager
		void draw_windows();
		// End frame
		// Call this after all drawing is done
		void end_frame();
		// Handle SDL2 events
		// Do NOT use if you want to handle events
		// yourself. This method forwards all
		// events to the GUI system.
		//
		// Returns true if user requested window close,
		// false otherwise.
		bool handle_events();
		void set_event_handler(const std::function<void(const SDL_Event& e)>& f) { m_event_handler = f; }

		// glViewport wrapper
		// Start and end coordinates relative to the UPPER-LEFT CORNER of the screen
		void set_viewport(int sx, int sy, int ex, int ey);
		// Restore viewport to fullscreen
		void restore_viewport();
		// Clears the screen
		void clear();
		// Set the clear color
		void clear_color(float r, float g, float b);

		// Get shader program index by name
		int get_shader_program_index(const std::string& name);

		// Store shader program
		size_t load_shader(shader_program* pShaderProgram);
		// Load shader from .qc file
		size_t load_shader(const std::string& filename);
		// Load texture from file
		uint32_t load_texture(const std::string& filename);
		// Load model from file
		gfx::model_id load_model(const std::string& filename);
		// Load model from model container
		gfx::model_id load_model(const gfx::model& mdl);
		// Load material from file
		size_t load_material(const std::string& filename);

		// Unload model
		void unload_model(model_id mdl);

		// Bind model
		void bind_model(model_id mdl);
		// Unbind model
		void unbind_model();

		// Draw currently bound model with the shader
		// currently in use.
		void draw_model();

		// value of -1 activates the shader assigned to the material
		// of the currently bound model
		long long int use_shader(long long int shader = -1);

		// Add gfx::window to the window manager
		// (does NOT store the window object)
		void add_window(std::shared_ptr<gfx::window> w) { windows.push_back(w); }
		// Removes window from the window manager by pointer
		void remove_window(std::shared_ptr<gfx::window> w);

		// Get shader with ID
		shader_program* get_shader(size_t i) { return shaders[i]; }

		// Get time elapsed since last and current frame
		float& delta() { return flDeltaTime; }
		// Get current engine time
		float& curtime() { return flCurrentTime; }

		// Get window width
		int width() const { return nWidth; }
		// Get window height
		int height() const { return nHeight; }

		// Get model vertex count
		size_t model_vertices(model_id mdl) const { return model_vertexcount_map.at(mdl); }
		// Does the model have a collider
		bool model_has_collider(model_id mdl) const { return model_collider_map.count(mdl) && model_collider_map.at(mdl).size() > 0; }
		// Get the material associated to the model
		const gfx::material& model_material(model_id mdl) const { return materials[model_material_map.at(mdl)]; }
		// Get filename of material associated to the model
		const std::string& model_material_filename(model_id mdl) const { return material_filename_map.at(model_material_map.at(mdl)); }

		// Check if GUI has keyboard focus
		bool gui_keyboard_focus() const;
		// Get vector of unhandled events
		void get_events(std::vector<SDL_Event>& v);
		// Send event to the GUI
		// (i.e. fake events or send event back) 
		void gui_send_event(const SDL_Event& e);

		void capture_mouse(bool b);

		void load_default_shaders();

		model_id upload_terrain(const elf::terrain_chunk& chunk);
		void draw_terrain(const model_id& id);

	private:
		SDL_Window* pWindow;
		SDL_GLContext pGLContext;
		SDL_Renderer* pRenderer;
		TTF_Font* pDebugFont;

		model_id current_model;

		uint64_t nTimePrev = 0, nTimeNow = 0;

		float flDeltaTime = 0;
		float flCurrentTime = 0;

		int nWidth, nHeight;

		int nViewportX = -1, nViewportY = -1, nViewportW = -1, nViewportH = -1;

		std::vector<shader_program*> shaders;
		std::map<std::string, size_t> shader_name_map;

		std::vector<std::shared_ptr<gfx::window>> windows;

		std::map<model_id, size_t> model_material_map;
		std::map<model_id, size_t> model_vertexcount_map;

		std::map<std::string, model_id> filename_model_map;
		std::map<model_id, std::vector<triangle>> model_collider_map;

		std::map<std::string, uint32_t> filename_texture_map;

		std::vector<gfx::material> materials;
		std::map<std::string, size_t> filename_material_map;
		std::map<size_t, std::string> material_filename_map;

		std::function<void(const SDL_Event& e)> m_event_handler;
	};
}

extern gfx::gfx_global* gpGfx;
