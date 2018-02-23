#include "stdafx.h"
#include "event_handler.h"
#include "input.h"
#include "camera.h"

#include <SDL2/SDL.h>

input::input()
{
	// default binds
	bind_key(SDLK_UP, IA_FORWARD);
	bind_key(SDLK_DOWN, IA_BACKWARD);
	bind_key(SDLK_LEFT, IA_STRAFE_L);
	bind_key(SDLK_RIGHT, IA_STRAFE_R);
	bind_key(SDLK_PERIOD, IA_TURN_L);
	bind_key(SDLK_COMMA, IA_TURN_R);
	bind_key(SDLK_SPACE, IA_JUMP);
}

void input::press_key(long int keysym)
{
	m_keystate[keysym] = true;
}

void input::release_key(long int keysym)
{
	m_keystate[keysym] = false;
}

void input::bind_key(long int keysym, input_action ia)
{
	m_keybinds.emplace(keysym, ia);
}

void input::update()
{
	for (auto& ks : m_keystate)
	{
		if (ks.second)
		{
			switch (m_keybinds[ks.first])
			{
				case IA_FORWARD:
					gpGlobals->pCamera->forward();
					break;
				case IA_BACKWARD:
					gpGlobals->pCamera->backward();
					break;
				case IA_TURN_L:
					gpGlobals->pCamera->turn_left();
					break;
				case IA_TURN_R:
					gpGlobals->pCamera->turn_right();
					break;
				case IA_STRAFE_L:
					gpGlobals->pCamera->strafe_left();
					break;
				case IA_STRAFE_R:
					gpGlobals->pCamera->strafe_right();
					break;
				case IA_JUMP:
					//gpGlobals->pCamera->jump();
					break;
			}
		}
	}
}
