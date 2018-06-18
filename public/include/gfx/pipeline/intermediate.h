#pragma once

#include <gfx/pipeline/stage.h>

namespace gfx::pipeline {

class intermediate : public stage {
public:
	gfx::shared_fb process(gfx::shared_fb input);
};
}
