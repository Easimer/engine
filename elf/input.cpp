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

	PRINT_DBG("input::input: default keybindings loaded");
}

void input::bind_key(long int keysym, const std::string& cmd) {
	m_keybinds.emplace(keysym, cmd);
}

void input::define_command(const std::string & cmd, const std::function<void()>& f) {
	m_commands.emplace(cmd, f);
}

void input::update() {
	auto pKeyState = SDL_GetKeyboardState(NULL);
	for (auto& keybind : m_keybinds) {
		bool bKeyState = pKeyState[SDL_GetScancodeFromKey(keybind.first)];
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
				} else if (ks.second == RISING) {
					if (m_commands.count(cmd))
						m_commands[cmd]();
					continue;
				} else if (ks.second == FALLING) {
					cmd[0] = '+';
					if (m_commands.count(cmd))
						m_commands[cmd]();
					continue;
				}
			} else if (ks.second == PRESSED) {
				// NOTE: this effectively spams the command,
				// although this is how it works in Source1
				// so I guess it's good enough
				if (m_commands.count(cmd))
					m_commands[cmd]();
			}
		}
	}
}

void input::mouse_motion(const int x, const int y) {
	/// First-Person
	if (!m_pCamera) return;
	m_pCamera->turn(x, y, gpGfx->delta());
}
