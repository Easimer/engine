cmake_minimum_required(VERSION 3.2.3)

#IF(WIN32)
#set(CMAKE_GENERATOR_TOOLSET "Windows7.1SDK" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
#set(CMAKE_VS_PLATFORM_TOOLSET "Windows7.1SDK" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
#ENDIF()

vs_pch(stdafx)

# Add source files here

set(SRC_CORE
	stdafx.cpp
	stdafx.h
	
	main.cpp

	globals.h
	statistics.h

	event_handler.cpp
	event_handler.h

	input.cpp
	input.h
)

set(SRC_ENTSYS
	baseentity.cpp
	entsys.cpp

	basethinker.h
	baseentity.h
	entityfactory.h
	entsys.h

	keyvalues.h
	
	entsearch.cpp
	entsearch.h
)

set(SRC_ENTSYS_PROP
	prop_common.h
	prop_dynamic.cpp
	prop_static.cpp
	prop_physics.cpp
)

set(SRC_ENTSYS_LIGHT
	light_base.h
	
	light_point.cpp
	light_point.h

	light_global.cpp
	light_global.h
)

set(SRC_GFX
	igfx.cpp
	igfx.h
)

set(SRC_NOPCH
)

set(SRC_RES_SHADERS
	shaders/model_dynamic.qc
	shaders/model_dynamic.frag
	shaders/model_dynamic.vert

	shaders/wireframe.qc
	shaders/wireframe.frag
	shaders/wireframe.vert

	shaders/depth_map.qc
	shaders/depth_map.frag
	shaders/depth_map.vert
)

set(SRC_GUI

	devgui.cpp
	devgui.h

	console.cpp
	console.h
)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

add_executable(game
	${SRC_CORE}
	${SRC_ENTSYS}
	${SRC_ENTSYS_PROP}
	${SRC_ENTSYS_LIGHT}
	${SRC_GFX}
	${SRC_RES_SHADERS}
	${SRC_NOPCH}
	${SRC_GUI}
	CMakeLists.txt
)

vs_link_static(game)

SET(GAME_LIBRARIES enl SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf imgui phys graphics networking)

IF(UNIX AND NOT APPLE)
target_link_libraries(game ${GAME_LIBRARIES} dl pthread)
ELSE()
target_link_libraries(game ${GAME_LIBRARIES})
ENDIF()
install(TARGETS game DESTINATION bin/)
install(FILES ${SRC_RES_SHADERS} DESTINATION data/shaders/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC_GAME})
source_group("Source Files\\Model Loader" FILES ${SRC_MDLLDR})
source_group("Source Files\\Entity System" FILES ${SRC_ENTSYS})
source_group("Source Files\\Entity System\\Props" FILES ${SRC_ENTSYS_PROP})
source_group("Source Files\\Entity System\\Lighting" FILES ${SRC_ENTSYS_LIGHT})
source_group("Source Files\\Graphics" FILES ${SRC_GFX})
source_group("Source Files\\GUI" FILES ${SRC_GUI})
source_group("Shader Source Files" FILES ${SRC_RES_SHADERS})
ENDIF()
