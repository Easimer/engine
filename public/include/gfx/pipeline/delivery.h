#pragma once

#include <gfx/pipeline/stage.h>
#include <gfx/framebuffer.h>

namespace gfx::pipeline {

class delivery : public stage {
public:
	delivery();
	void process(gfx::shared_fb input, void* pMatProj);

	size_t color() const noexcept override { return 0; }
	size_t normal() const noexcept override { return 0; }
	size_t worldpos() const noexcept override { return 0; }
	size_t specular() const noexcept override { return 0; }
private:
	gfx::shader_program* m_shader;
};

}