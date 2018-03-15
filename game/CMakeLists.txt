cmake_minimum_required(VERSION 3.2.3)

#IF(WIN32)
#set(CMAKE_GENERATOR_TOOLSET "Windows7.1SDK" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
#set(CMAKE_VS_PLATFORM_TOOLSET "Windows7.1SDK" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
#ENDIF()

project(klog)

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
)

set(SRC_ENTSYS_LIGHT
	light.h

	light_base.h
	
	light_point.cpp
	light_point.h

	light_global.cpp
	light_global.h
)

set(SRC_GFX
	renderer.cpp
	renderer.h

	shader.cpp
	shader.h

	shader_program.cpp
	shader_program.h

	icamera.h
	camera.cpp
	camera.h

	../public/src/qc_parser.cpp
	../public/include/qc_parser.h

	../public/src/smd_parser.cpp
	../public/include/smd_parser.h

	vector.cpp
	vector.h

	material.cpp
	material.h
	
	../public/src/emf_loader.cpp
	../public/include/emf_loader.h

	../public/include/octree.h
)

set(SRC_NOPCH
	glad.c
	gui/imgui_impl_sdl_gl3.cpp
	gui/imgui_impl_sdl_gl3.h
)

set(SRC_RES_SHADERS
	shaders/model_dynamic.qc
	shaders/model_dynamic.frag
	shaders/model_dynamic.vert
)

set(SRC_GUI
	gui/imgui_impl_sdl_gl3.cpp
	gui/imgui_impl_sdl_gl3.h

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

target_link_libraries(game enl SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf imgui)
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
