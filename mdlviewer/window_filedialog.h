#pragma once

#include <gfx/window.h>
#include <gfx/material.h>

class window_filedialog : public gfx::window {
public:
	window_filedialog(const char* init_path);
	virtual const char * get_title() override { return "Open file"; }
protected:
	virtual void draw_content() override;
};
