#include "stdafx.h"
#include "events.h"

void event_handler::send_events(const std::vector<SDL_Event>& events) {
	for (auto& ev : events)
		handle_event(ev);
}

void event_handler::handle_event(const SDL_Event& ev) {
	if (!m_pInput)
		return;
	switch (ev.type) {
	case SDL_MOUSEMOTION:
		m_pInput->mouse_motion(ev.motion.xrel, ev.motion.yrel);
		break;
	}
}
