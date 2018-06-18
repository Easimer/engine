#pragma once

#include <gfx/pipeline/stage.h>

namespace gfx::pipeline {

class intermediate : public stage {
public:
	intermediate(const std::string& filename);
	gfx::shared_fb process(gfx::shared_fb input);

	const std::string& name() const noexcept { return m_name; }
	const bool intake_post() const noexcept { return m_intake_post; }
private:
	std::string m_name;
	bool m_intake_post;
	gfx::shader_program* m_shader;
	gfx::shared_fb m_framebuffer;
};
}
