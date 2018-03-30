#pragma once

#if defined(PLAT_DEBUG)
#ifndef RESTRICT_THREAD_RENDERING
#define RESTRICT_THREAD_RENDERING
#endif
#define ASSERT_OPENGL() \
	{ \
		RESTRICT_THREAD_RENDERING; \
		GLenum iErr = glGetError(); \
		if (iErr != GL_NO_ERROR) \
		{ \
			switch(iErr) { \
				case GL_INVALID_OPERATION: \
					PRINT_ERR("OpenGL Invalid Operation!"); \
					break; \
				case GL_INVALID_ENUM: \
					PRINT_ERR("OpenGL Invalid Enum!");\
					break; \
				case GL_INVALID_VALUE: \
					PRINT_ERR("OpenGL Invalid Value!");\
					break; \
				default: \
					PRINT_ERR("OpenGL error: " << iErr); \
					break; \
			} \
			ASSERT(iErr == GL_NO_ERROR); \
		} \
	}
#else
#define ASSERT_OPENGL()
#endif
