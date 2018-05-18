cmake_minimum_required(VERSION 3.2.3)

vs_pch(stdafx)

# Add source files here

set(SRC_CORE
	stdafx.cpp
	stdafx.h
	
	server_browser.cpp

	server_list.cpp
	server_list.h

	renderer.cpp
	renderer.h
)

set(SRC_NOPCH
	glad.c
	glad/glad.h
	
	gui/imgui_impl_sdl_gl3.cpp
	gui/imgui_impl_sdl_gl3.h
)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

add_executable(server_browser
	${SRC_CORE}
	${SRC_NOPCH}
	CMakeLists.txt
)

vs_link_static(server_browser)

SET(GAME_LIBRARIES networking graphics AdvApi32 SDL2main SDL2 imgui)

IF(UNIX AND NOT APPLE)
target_link_libraries(server_browser ${GAME_LIBRARIES} dl pthread)
ELSE()
target_link_libraries(server_browser ${GAME_LIBRARIES})
ENDIF()
install(TARGETS server_browser DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC_CORE} ${SRC_NOPCH})
ENDIF()
