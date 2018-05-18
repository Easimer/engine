#pragma once

#include <SDL2/SDL.h>

class renderer {
public:
	renderer();
	~renderer();

	
	renderer(const renderer&) = delete;
	void operator=(const renderer&) = delete;

	void begin_frame();
	void end_frame();

	bool handle_events();

private:
	SDL_GLContext m_pGLContext;
	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;
};
