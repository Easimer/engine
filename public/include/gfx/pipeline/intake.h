#pragma once

#include <gfx/gfx.h>
#include <gfx/pipeline/pipeline.h>
#include <gfx/pipeline/stage.h>
#include <glm/fwd.hpp>
#include <gfx/framebuffer.h>

namespace gfx::pipeline {

struct draw_order;

class intake : public stage {
public:
	intake();
	void begin();
	void draw(const draw_order& cmd);
	void finalize();
	gfx::shared_fb framebuffer() const { return m_framebuffer; }
private:
	gfx::shared_fb m_framebuffer;
};

}
