#pragma once

#include <gfx/pipeline/stage.h>
#include <gfx/framebuffer.h>

namespace gfx::pipeline {

class delivery : public stage {
public:
	delivery() : stage(true) {}
	void process(gfx::shared_fb input);
};

}