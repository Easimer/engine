#pragma once

#include <gfx/pipeline/stage.h>
#include <gfx/light.h>

namespace gfx::pipeline {

constexpr size_t lights_limit = 4;

class intermediate : public stage {
public:
	intermediate() : stage(false) {}
	intermediate(const std::string& filename);
	gfx::shared_fb process(gfx::shared_fb input);
	gfx::shared_fb process(const std::array<gfx::shader_light, lights_limit>&, gfx::shared_fb input);

	const std::string& name() const noexcept { return m_name; }
	bool lit() const noexcept { return m_lit; }

	size_t color() const noexcept override { return m_framebuffer1->diffuse()->handle(); }
	size_t normal() const noexcept override { return m_framebuffer1->normal()->handle(); }
	size_t worldpos() const noexcept override { return m_framebuffer1->worldpos()->handle(); }
	size_t specular() const noexcept override { return m_framebuffer1->specular()->handle(); }
	size_t selfillum() const noexcept override { return m_framebuffer1->selfillum()->handle(); }
private:
	std::string m_name;
	gfx::shader_program* m_shader;
	gfx::shared_fb m_framebuffer1, m_framebuffer2;
	bool m_lit;
};
}
