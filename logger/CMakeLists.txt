cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	main.cpp
	
	stdafx.cpp
	stdafx.h
	
	renderer.cpp
	renderer.h
)

set(SRC_NOPCH
	glad.c
	glad/glad.h
	
	gui/imgui_impl_sdl_gl3.cpp
	gui/imgui_impl_sdl_gl3.h
)

set(SRC_PUBLIC
	../public/src/logger_server.cpp
	../public/include/logger_server.h
	../public/include/logger_shared.h
)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

vs_pch(stdafx)

add_executable(logger
	${SRC}
	${SRC_PUBLIC}
	${SRC_NOPCH}
	CMakeLists.txt
)

target_link_libraries(logger Ws2_32 Mswsock AdvApi32 SDL2main SDL2 imgui)
install(TARGETS logger DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()
