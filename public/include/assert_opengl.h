#pragma once

#if defined(PLAT_DEBUG)
#define ASSERT_OPENGL() \
	{ \
		RESTRICT_THREAD_RENDERING; \
		GLenum iErr = glGetError(); \
		if (iErr != GL_NO_ERROR) \
		{ \
			PRINT_ERR("OpenGL error: " << iErr); \
			ASSERT(0); \
		} \
	}
#else
#define ASSERT_OPENGL()
#endif
