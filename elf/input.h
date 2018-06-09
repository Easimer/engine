#pragma once

#include <map>
#include <functional>
#include <gfx/icamera.h>

enum input_action {
	IA_FORWARD = 0,
	IA_BACKWARD = 1,
	IA_STRAFE_L = 2,
	IA_STRAFE_R = 3,
	IA_TURN_L = 4,
	IA_TURN_R = 5,
	IA_JUMP = 6,
	IA_MAX = 7
};

class input {
public:
	input();
	void assign_camera(gfx::icamera* cam) { m_pCamera = cam; }
	void bind_key(long int keysym, const std::string& cmd);
	void define_command(const std::string& cmd, const std::function<void()>& f);
	void update();
	void mouse_motion(const int x, const int y);

	enum action_state {
		RISING, // The button state transitions from released to pressed
		PRESSED, // The button is held
		FALLING, // The button state transitions from pressed to released
		RELEASED // The button is not held
	};

private:
	//std::map<long int, std::pair<std::string, std::function<void>>> m_keybinds;
	//std::map<input_action, action_state> m_action_state;

	// Maps keys to a state
	std::map<long int, action_state> m_keystates;
	// Maps keys to a command
	std::map<long int, std::string> m_keybinds;
	// Maps commands to a client-side action
	std::map<std::string, std::function<void()>> m_commands;

	bool m_bMouseCaptured = false;

	int m_nScreenWidth = -1, m_nScreenHeight = -1;
	int m_nMouseLastX = -1, m_nMouseLastY = -1;

	gfx::icamera* m_pCamera;
};
