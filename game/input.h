#pragma once

#include <map>

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
	void press_key(long int keysym);
	void release_key(long int keysym);
	void bind_key(long int keysym, input_action ia);
	void update();

protected:
	void check_conflicting_actions(const input_action&);

private:
	std::map<long int, input_action> m_keybinds;
	std::map<input_action, bool> m_action_state;
};
