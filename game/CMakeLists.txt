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

set(SRC_GAME
	stdafx.cpp
	stdafx.h
	
	main.cpp
	baseentity.cpp
	entsys.cpp

	globals.h
	vector.h
	basethinker.h
	baseentity.h
	entityfactory.h
	entsys.h
)

set(SRC_MDLLDR
	qc_parser.cpp
	smd_parser.cpp
	
	qc_parser.h
	smd_parser.h
)

set(SRC_NOPCH
	glad.c
)

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
add_definitions(-DDEBUG)
ENDIF()

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

set(CMAKE_CXX_STANDARD 11)

add_executable(game
	${SRC_GAME}
	${SRC_MDLLDR}
	${SRC_NOPCH}
	CMakeLists.txt
)

target_link_libraries(game enl)
install(TARGETS game DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC_GAME})
source_group("Source Files\\Model Loader" FILES ${SRC_MDLLDR})
ENDIF()
