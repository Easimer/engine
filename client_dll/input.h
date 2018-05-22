#pragma once

#include <map>
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
	void press_key(long int keysym);
	void release_key(long int keysym);
	void bind_key(long int keysym, input_action ia);
	void update();
	void mouse_motion(const int x, const int y);

protected:
	void check_conflicting_actions(const input_action&);

private:
	std::map<long int, input_action> m_keybinds;
	std::map<input_action, bool> m_action_state;

	bool m_bMouseCaptured = false;

	int m_nScreenWidth = -1, m_nScreenHeight = -1;
	int m_nMouseLastX = -1, m_nMouseLastY = -1;

	gfx::icamera* m_pCamera;
};
