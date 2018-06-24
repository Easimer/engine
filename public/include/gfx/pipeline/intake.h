#pragma once

#include <gfx/gfx.h>
#include <gfx/pipeline/pipeline.h>
#include <gfx/pipeline/stage.h>
#include <glm/fwd.hpp>
#include <gfx/framebuffer.h>

namespace gfx::pipeline {

class idraw_order;

class intake : public stage {
public:
	intake();
	void begin(bool bGUI);
	void draw(const gfx::pipeline::idraw_order& cmd);
	void finalize();
	gfx::shared_fb framebuffer() const { return m_framebuffer; }

	size_t color() const noexcept override { return m_framebuffer->diffuse()->handle(); }
	size_t normal() const noexcept override { return m_framebuffer->normal()->handle(); }
	size_t worldpos() const noexcept override { return m_framebuffer->worldpos()->handle(); }
	size_t specular() const noexcept override { return m_framebuffer->specular()->handle(); }
	size_t selfillum() const noexcept override { return m_framebuffer->selfillum()->handle(); }
private:
	gfx::shared_fb m_framebuffer;
};

}
