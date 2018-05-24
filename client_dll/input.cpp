#include "stdafx.h"
#include "input.h"
#include <gfx/gfx.h>
#include <gfx/camera.h>

#include <algorithm>

#include <SDL2/SDL.h>

input::input() : m_pCamera(nullptr) {
	// action states
	m_keystates[SDLK_w] = RELEASED;
	m_keystates[SDLK_s] = RELEASED;
	m_keystates[SDLK_a] = RELEASED;
	m_keystates[SDLK_d] = RELEASED;
	m_keystates[SDLK_SPACE] = RELEASED;

	bind_key(SDLK_w, "+forward");
	bind_key(SDLK_s, "+backward");
	bind_key(SDLK_a, "+moveleft");
	bind_key(SDLK_d, "+moveright");
	bind_key(SDLK_SPACE, "+jump");

	define_command("+forward", std::function<void()>([&]() {
		if (!m_pCamera) return;
		m_pCamera->forward(gpGfx->delta());
	}));
	define_command("+backward", [&]() {
		if (!m_pCamera) return;
		m_pCamera->backward(gpGfx->delta());
	});
	define_command("+moveleft", [&]() {
		if (!m_pCamera) return;
		m_pCamera->strafe_left(gpGfx->delta());
	});
	define_command("+moveright", [&]() {
		if (!m_pCamera) return;
		m_pCamera->strafe_right(gpGfx->delta());
	});
	/*for (size_t i = 0; i < IA_MAX; i++)
		m_action_state[(input_action)i] = false;
	// default binds
	bind_key(SDLK_w, IA_FORWARD);
	bind_key(SDLK_s, IA_BACKWARD);
	bind_key(SDLK_a, IA_STRAFE_L);
	bind_key(SDLK_d, IA_STRAFE_R);
	bind_key(SDLK_PERIOD, IA_TURN_L);
	bind_key(SDLK_COMMA, IA_TURN_R);
	bind_key(SDLK_SPACE, IA_JUMP);*/
	
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

void input::bind_key(long int keysym, const std::string& cmd) {
	m_keybinds.emplace(keysym, cmd);
}

void input::define_command(const std::string & cmd, const std::function<void()>& f) {
	m_commands.emplace(cmd, f);
}

std::vector<std::string> input::update() {
	//if (gpGlobals->bDevGUI)
	//	return;
	auto pKeyState = SDL_GetKeyboardState(NULL);
	for (auto& keybind : m_keybinds) {
		bool bKeyState = pKeyState[SDL_GetScancodeFromKey(keybind.first)];
		//action_state& as = m_action_state[keybind.first];
		action_state& as = m_keystates[keybind.first];
		if (bKeyState) {
			switch (as) {
			case RELEASED:
				as = RISING;
				break;
			case RISING:
				as = PRESSED;
				break;
			case FALLING:
				as = PRESSED;
				break;
			// PRESSED -> PRESSED
			}
		} else {
			switch (as) {
			case PRESSED:
				as = FALLING;
				break;
			case FALLING:
				as = RELEASED;
				break;
			case RISING: // Button was only held for one frame
				as = FALLING;
				break;
			// RELEASED -> RELEASED
			}
		}
	}
	std::vector<std::string> commands;

	// Enumerate all keys that we have info about their state:
	// 1.	Is the key bound to a command? If not: CONTINUE
	// 2.	Acquire the command bound to the key
	// 3.	Is it a toggleable command? If so: GOTO 4; otherwise: GOTO 5
	// 4.a	Is it RISING? If so: add command as POSITIVE and CONTINUE
	// 4.b	Is it FALLING? if so: add command as NEGATIVE and CONTINUE
	// 5.	Add command and CONTINUE
	for (auto& ks : m_keystates) {
		if (m_keybinds.count(ks.first)) {
			std::string& cmd = m_keybinds[ks.first];
			if (cmd[0] == '+') {
				if (ks.second == PRESSED) {
					if (m_commands.count(cmd))
						m_commands[cmd]();
					continue;
				}
				else if (ks.second == RISING) {
					commands.push_back(cmd);
					if (m_commands.count(cmd))
						m_commands[cmd]();
					continue;
				}
				else if (ks.second == FALLING) {
					cmd[0] = '-';
					commands.push_back(cmd);
					cmd[0] = '+';
					if (m_commands.count(cmd))
						m_commands[cmd]();
					continue;
				}
			} else if(ks.second == PRESSED) {
				// NOTE: this effectively spams the command,
				// although this is how it works in Source1
				// so I guess it's good enough
				commands.push_back(cmd);
				if(m_commands.count(cmd))
					m_commands[cmd]();
			}
		}
	}

	return commands;
}

void input::mouse_motion(const int x, const int y) {
	/// First-Person
	if (!m_pCamera) return;
	m_pCamera->turn(x, y, gpGfx->delta());
}

void input::check_conflicting_actions(const input_action& act) {
	//switch (act) {
	//case IA_FORWARD:
	//	m_action_state[IA_BACKWARD] = false;
	//	break;
	//case IA_BACKWARD:
	//	m_action_state[IA_FORWARD] = false;
	//	break;
	//case IA_TURN_L:
	//	m_action_state[IA_TURN_R] = false;
	//	break;
	//case IA_TURN_R:
	//	m_action_state[IA_TURN_L] = false;
	//	break;
	//case IA_STRAFE_L:
	//	m_action_state[IA_STRAFE_R] = false;
	//	break;
	//case IA_STRAFE_R:
	//	m_action_state[IA_STRAFE_L] = false;
	//	break;
	//}
}
