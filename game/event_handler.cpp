#include "stdafx.h"
#include "event_handler.h"

void event_handler::push_event(std::vector<event_t>& events)
{
	RESTRICT_THREAD_RENDERING; // SDL2 window events come from the thread handling the window

	m_event_buf.BeginWrite();
	for (size_t i = 0; i < events.size(); i++)
	{
		m_event_buf.Write(events[i]);
	}
	m_event_buf.EndWrite();
}

void event_handler::update()
{
	RESTRICT_THREAD_LOGIC;
	size_t nEvents = 0;
	event_t* pEvents;
	if (!m_event_buf.BeginRead(&pEvents, &nEvents))
	{
		return;
	}

	if (nEvents == 0) {
		m_event_buf.EndRead();
		return;
	}

	while (nEvents--)
	{
		SDL_Event event = pEvents->event;
		switch (event.type)
		{
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						gpGlobals->bRunning = false;
						PRINT_DBG("Shutdown triggered!");
						break;
					default:
						PRINT_DBG("Unhandled window event: " << (int)event.window.event);
						break;
				}
				break;
			default:
				PRINT_DBG("Unhandled event: " << (int)event.type);
				break;
		}
	}

	m_event_buf.EndRead();
}
