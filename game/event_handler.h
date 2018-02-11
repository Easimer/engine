#pragma once

#include <cmdbuf.h>
#include <SDL2/SDL.h>
#include <vector>

CMDBUF_BEGIN_CMD(event_t)
	SDL_Event event;
CMDBUF_END_CMD(event_t)

CMDBUF_DEF(event_buf, event_t, 32, true, false);

class event_handler {
public:
	void push_event(std::vector<event_t>& events);
	void update();
private:
	event_buf m_event_buf;
};
