#pragma once

#include "common.h"
#include <glad/glad.h>

#if defined(PLAT_DEBUG)
#define ASSERT_OPENGL() \
	{ \
		RESTRICT_DRAW_THREAD; \
		GLenum iErr = glGetError(); \
		if (iErr != GL_NO_ERROR) \
		{ \
			Err("OpenGL error: %u\n", iErr); \
			ASSERT(0); \
		} \
	}
#else
#define ASSERT_OPENGL()
#endif
