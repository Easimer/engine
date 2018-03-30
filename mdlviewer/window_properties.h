#pragma once

#include <gfx/window.h>

class window_properties : public gfx::window {
public:
	virtual const char * get_title() override { return "Properties"; }
protected:
	virtual void draw_content() override;
};
