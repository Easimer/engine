#pragma once

#include <vector>
#include "input.h"
#include <SDL2/SDL.h>

class event_handler {
public:
	event_handler() : m_pInput(nullptr) {}
	void assign_input_handler(input* inp) { m_pInput = inp; }

	inline void send_event(const SDL_Event& ev) { handle_event(ev); }
	void send_events(const std::vector<SDL_Event>& events);
private:
	void handle_event(const SDL_Event& ev);
	input* m_pInput;
};
