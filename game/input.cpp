#include "stdafx.h"
#include "event_handler.h"
#include "input.h"
#include "icamera.h"
#include "renderer.h"

#include <algorithm>

#include <SDL2/SDL.h>

input::input()
{
	// action states
	for (size_t i = 0; i < IA_MAX; i++)
		m_action_state[(input_action)i] = false;
	// default binds
	bind_key(SDLK_w, IA_FORWARD);
	bind_key(SDLK_s, IA_BACKWARD);
	bind_key(SDLK_a, IA_STRAFE_L);
	bind_key(SDLK_d, IA_STRAFE_R);
	bind_key(SDLK_PERIOD, IA_TURN_L);
	bind_key(SDLK_COMMA, IA_TURN_R);
	bind_key(SDLK_SPACE, IA_JUMP);
	PRINT_DBG("input::input: default keybindings loaded");
}

void input::press_key(long int keysym)
{
	//if (m_keybinds.count(keysym) == 0)
	//	return;
	//const auto& ia = m_keybinds[keysym];
	//m_action_state[ia] = true;
	//check_conflicting_actions(ia);
}

void input::release_key(long int keysym)
{
	//if (m_keybinds.count(keysym) == 0)
	//	return;
	//m_action_state[m_keybinds[keysym]] = false;

	if (keysym == SDLK_ESCAPE) {
		if (SDL_CaptureMouse(m_bMouseCaptured ? SDL_FALSE : SDL_TRUE) == -1)
		{
			ASSERT_SDL2(0);
		}
		m_bMouseCaptured = !m_bMouseCaptured;
	}
}

void input::bind_key(long int keysym, input_action ia)
{
	m_keybinds.emplace(keysym, ia);
}

void input::update()
{
	auto pKeyState = SDL_GetKeyboardState(NULL);
	for (auto& keybind : m_keybinds)
	{
		m_action_state[keybind.second] = pKeyState[SDL_GetScancodeFromKey(keybind.first)];
	}
	for (auto& ks : m_action_state)
	{
		if (ks.second) // is action active
		{
			switch (ks.first)
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

void input::mouse_motion(const int x, const int y)
{
	/// First-Person

	gpGlobals->pCamera->turn(x, y);

}

void input::check_conflicting_actions(const input_action& act)
{
	switch (act)
	{
		case IA_FORWARD:
			m_action_state[IA_BACKWARD] = false;
			break;
		case IA_BACKWARD:
			m_action_state[IA_FORWARD] = false;
			break;
		case IA_TURN_L:
			m_action_state[IA_TURN_R] = false;
			break;
		case IA_TURN_R:
			m_action_state[IA_TURN_L] = false;
			break;
		case IA_STRAFE_L:
			m_action_state[IA_STRAFE_R] = false;
			break;
		case IA_STRAFE_R:
			m_action_state[IA_STRAFE_L] = false;
			break;
	}
}
