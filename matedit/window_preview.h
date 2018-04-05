#pragma once

#include <gfx/window.h>

class window_preview : public gfx::window {
public:
	virtual const char * get_title() override { return "Preview"; }
protected:
	virtual void draw_content() override;
};
