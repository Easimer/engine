#include "stdafx.h"
#include "input.h"
#include <gfx/gfx.h>
#include <gfx/camera.h>

#include <algorithm>

#include <SDL2/SDL.h>

input::input() : m_pCamera(nullptr) {
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

void input::press_key(long int keysym) {
	if (!m_pCamera) return;
	//if (m_keybinds.count(keysym) == 0)
	//	return;
	//const auto& ia = m_keybinds[keysym];
	//m_action_state[ia] = true;
	//check_conflicting_actions(ia);

	if (keysym == SDLK_LSHIFT) {
		//m_pCamera->set_speed(1.6);
	}
}

void input::release_key(long int keysym) {
	if (!m_pCamera) return;
	//if (m_keybinds.count(keysym) == 0)
	//	return;
	//m_action_state[m_keybinds[keysym]] = false;
	if (keysym == SDLK_LSHIFT) {
		//m_pCamera->set_speed(0.8);
	}
}

void input::bind_key(long int keysym, input_action ia) {
	m_keybinds.emplace(keysym, ia);
}

void input::update() {
	//if (gpGlobals->bDevGUI)
	//	return;
	auto pKeyState = SDL_GetKeyboardState(NULL);
	for (auto& keybind : m_keybinds) {
		m_action_state[keybind.second] = pKeyState[SDL_GetScancodeFromKey(keybind.first)];
	}
	if (!m_pCamera) return;
	for (auto& ks : m_action_state) {
		if (ks.second) // is action active
		{
			switch (ks.first) {
			case IA_FORWARD:
				
				m_pCamera->forward(gpGfx->delta());
				break;
			case IA_BACKWARD:
				m_pCamera->backward(gpGfx->delta());
				break;
			case IA_TURN_L:
				m_pCamera->turn_left(gpGfx->delta());
				break;
			case IA_TURN_R:
				m_pCamera->turn_right(gpGfx->delta());
				break;
			case IA_STRAFE_L:
				m_pCamera->strafe_left(gpGfx->delta());
				break;
			case IA_STRAFE_R:
				m_pCamera->strafe_right(gpGfx->delta());
				break;
			case IA_JUMP:
				//gpGlobals->pCamera->jump();
				break;
			}
		}
	}
}

void input::mouse_motion(const int x, const int y) {
	/// First-Person
	if (!m_pCamera) return;
	m_pCamera->turn(x, y, gpGfx->delta());
}

void input::check_conflicting_actions(const input_action& act) {
	switch (act) {
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
