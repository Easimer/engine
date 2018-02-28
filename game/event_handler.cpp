#include "stdafx.h"
#include "event_handler.h"
#include "input.h"
#include "gui/imgui_impl_sdl_gl3.h"

void event_handler::push_event(std::vector<event_t>& events)
{
	RESTRICT_THREAD_RENDERING; // SDL2 window events come from the thread handling the window

	m_event_buf.begin_write();
	for (size_t i = 0; i < events.size(); i++)
	{
		m_event_buf.write(events[i]);
	}
	m_event_buf.end_write();
}

void event_handler::update()
{
	RESTRICT_THREAD_LOGIC;
	size_t nEvents = 0;
	event_t* pEvents;
	if (!m_event_buf.begin_read(&pEvents, &nEvents))
	{
		return;
	}

	if (nEvents == 0) {
		m_event_buf.end_read();
		return;
	}

	while (nEvents--)
	{
		SDL_Event& event = pEvents->event;
		switch (event.type)
		{
			case SDL_KEYDOWN:
				if (event.key.repeat)
					break;
				if (ImGui_ImpSdlGL3_KeyboardFocused())
					break;
				gpGlobals->pInput->press_key(event.key.keysym.sym);
				break;
			case SDL_KEYUP:
				if (event.key.repeat)
					break;
				if (ImGui_ImpSdlGL3_KeyboardFocused())
					break;
				gpGlobals->pInput->release_key(event.key.keysym.sym);
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						gpGlobals->bRunning = false;
						PRINT_DBG("Shutdown triggered!");
						break;
					default:
						//PRINT_DBG("Unhandled window event: " << (int)event.window.event);
						break;
				}
				break;
			case SDL_MOUSEMOTION:
				gpGlobals->pInput->mouse_motion(event.motion.xrel, event.motion.yrel);
				break;
			default:
				//PRINT_DBG("Unhandled event: " << (int)event.type);
				break;
		}
	}

	m_event_buf.end_read();
}
