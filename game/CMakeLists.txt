cmake_minimum_required(VERSION 3.2.3)

#IF(WIN32)
#set(CMAKE_GENERATOR_TOOLSET "Windows7.1SDK" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
#set(CMAKE_VS_PLATFORM_TOOLSET "Windows7.1SDK" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
#ENDIF()

project(klog)

IF(WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
ENDIF()

vs_pch(stdafx)

# Add source files here

set(SRC_GAME
	stdafx.cpp
	stdafx.h
	
	main.cpp
)

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
add_definitions(-DDEBUG)
ENDIF()

set(CMAKE_CXX_STANDARD 11)

add_executable(game
	${SRC_GAME}
	CMakeLists.txt
)

target_link_libraries(game enl)
install(TARGETS game DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC_GAME})
ENDIF()
