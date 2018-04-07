cmake_minimum_required(VERSION 3.2.3)

vs_pch(stdafx)

# Add source files here

set(SRC_CORE
	stdafx.cpp
	stdafx.h
	
	server.cpp
	server.h

	globals.h
)

set(SRC_ENTSYS
	base_entity.cpp
	base_entity.h
	base_thinker.h
	entityfactory.h
	entity_system.cpp
	entity_system.h
	keyvalues.h
)

set(SRC_ENTSYS_PROP
)

set(SRC_ENTSYS_LIGHT
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
	../public/include/iserver.h
)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

add_library(server_dll SHARED
	${SRC_CORE}
	${SRC_ENTSYS}
	${SRC_ENTSYS_PROP}
	${SRC_ENTSYS_LIGHT}
	${SRC_GFX}
	${SRC_RES_SHADERS}
	${SRC_NOPCH}
	${SRC_GUI}
	${SRC_PUBLIC}
	CMakeLists.txt
)

vs_link_static(server_dll)

SET(GAME_LIBRARIES enl phys networking)

IF(UNIX AND NOT APPLE)
target_link_libraries(server_dll ${GAME_LIBRARIES} dl pthread)
ELSE()
target_link_libraries(server_dll ${GAME_LIBRARIES})
ENDIF()
install(TARGETS server_dll DESTINATION bin/)

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
