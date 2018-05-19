set(SRC
	stdafx.h
	stdafx.cpp

	camera.cpp
	framebuffer.cpp
	gfx.cpp
	material.cpp
	shader_program.cpp
	shader.cpp
	textures.cpp
	window.cpp
	window_register.cpp
	
	emf_loader.cpp
	smd_loader.cpp
)

set(SRC_NOPCH
	glad/glad.c
	gui/imgui_impl_sdl_gl3.cpp
)

set(SRC_PUBLIC
	../public/include/gfx/smd_loader.h
	../public/include/gfx/emf_loader.h

	../public/include/gfx/camera.h
	../public/include/gfx/framebuffer.h
	../public/include/gfx/gfx.h
	../public/include/gfx/icamera.h
	../public/include/gfx/light.h
	../public/include/gfx/material.h
	../public/include/gfx/model.h
	../public/include/gfx/shader_program.h
	../public/include/gfx/shader.h
	../public/include/gfx/textures.h
	../public/include/gfx/window.h
	../public/include/gfx/window_register.h
)

vs_pch(stdafx)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

set(CMAKE_CXX_STANDARD 17)

add_library(graphics STATIC
	${SRC}
	${SRC_NOPCH}
	${SRC_PUBLIC}
	CMakeLists.txt
)

target_link_libraries(graphics SDL2 SDL2_image)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
source_group("Public Headers" FILES ${SRC_PUBLIC})
ENDIF()

vs_link_no(graphics)

