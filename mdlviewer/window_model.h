#pragma once

#include <gfx/window.h>
#include <gfx/material.h>

class window_model : public gfx::window {
public:
	//window_model(const model& mdl, const gfx::material& mat);
	virtual const char * get_title() override { return "Model"; }
protected:
	virtual void draw_content() override;
};
