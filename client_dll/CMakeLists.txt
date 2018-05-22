cmake_minimum_required(VERSION 3.2.3)

vs_pch(stdafx)

# Add source files here

set(SRC_CORE
	stdafx.cpp
	stdafx.h
	
	client.cpp
	client.h

	mainmenu.cpp
	mainmenu.h

	game.cpp
	game.h

	base_entity.cpp
	base_entity.h

	input.cpp
	input.h

	events.cpp
	events.h

	globals.h
)

set(SRC_GFX
)

set(SRC_NOPCH
)

set(SRC_RES_SHADERS
)

set(SRC_GUI
)

set(SRC_PUBLIC
	../public/include/iclient.h
)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

add_library(client_dll SHARED
	${SRC_CORE}
	${SRC_GFX}
	${SRC_RES_SHADERS}
	${SRC_NOPCH}
	${SRC_GUI}
	${SRC_PUBLIC}
	CMakeLists.txt
)

vs_link_static(client_dll)

SET(GAME_LIBRARIES enl phys networking graphics imgui)

IF(UNIX AND NOT APPLE)
target_link_libraries(client_dll ${GAME_LIBRARIES} dl pthread)
ELSE()
target_link_libraries(client_dll ${GAME_LIBRARIES})
ENDIF()
install(TARGETS client_dll DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC_GAME})
source_group("Source Files\\Model Loader" FILES ${SRC_MDLLDR})
source_group("Source Files\\Graphics" FILES ${SRC_GFX})
source_group("Source Files\\GUI" FILES ${SRC_GUI})
source_group("Shader Source Files" FILES ${SRC_RES_SHADERS})
ENDIF()

# Copy DLL to launcher folder for testing
add_custom_command(TARGET client_dll POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:client_dll> ${CMAKE_BINARY_DIR}/launcher/bin/)
