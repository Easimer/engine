cmake_minimum_required(VERSION 3.2.3)

#IF(WIN32)
#set(CMAKE_GENERATOR_TOOLSET "Windows7.1SDK" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
#set(CMAKE_VS_PLATFORM_TOOLSET "Windows7.1SDK" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
#ENDIF()

project(klog)

IF(WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /Zi")
ENDIF()

vs_pch(stdafx)

# Add source files here

set(SRC_CORE
	stdafx.cpp
	stdafx.h
	
	main.cpp
	memstat.cpp
	memstat.h

	globals.h
	vector.h

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

	prop_common.h
	prop_dynamic.cpp
)

set(SRC_MDLLDR
	qc_parser.cpp
	smd_parser.cpp
	
	qc_parser.h
	smd_parser.h
)

set(SRC_GFX
	renderer.cpp
	renderer.h

	shader.cpp
	shader.h

	shader_program.cpp
	shader_program.h

	camera.cpp
	camera.h
)

set(SRC_NOPCH
	glad.c
)

set(SRC_RES_SHADERS
	shaders/model_dynamic.qc
	shaders/model_dynamic.frag
	shaders/model_dynamic.vert
)

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
add_definitions(-DDEBUG)
ENDIF()

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

set(CMAKE_CXX_STANDARD 17)

add_executable(game
	${SRC_CORE}
	${SRC_ENTSYS}
	${SRC_MDLLDR}
	${SRC_GFX}
	${SRC_RES_SHADERS}
	${SRC_NOPCH}
	CMakeLists.txt
)

target_link_libraries(game enl SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf)
install(TARGETS game DESTINATION bin/)
install(FILES ${SRC_RES_SHADERS} DESTINATION data/shaders/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC_GAME})
source_group("Source Files\\Model Loader" FILES ${SRC_MDLLDR})
source_group("Source Files\\Entity System" FILES ${SRC_ENTSYS})
source_group("Source Files\\Graphics" FILES ${SRC_GFX})
source_group("Shader Source Files" FILES ${SRC_RES_SHADERS})
ENDIF()
