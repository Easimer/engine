cmake_minimum_required(VERSION 3.2.3)

vs_pch(stdafx)

# Add source files here

set(SRC_CORE
	stdafx.cpp
	stdafx.h
	
	main.cpp
)

set(SRC_PUBLIC
	../public/include/iserver.h
	../public/include/iclient.h
	../public/include/dl.h
)

add_executable(launcher
	${SRC_CORE}
	${SRC_PUBLIC}
	CMakeLists.txt
)

vs_link_static(launcher)

IF(UNIX AND NOT APPLE)
target_link_libraries(launcher dl pthread)
ENDIF()
install(TARGETS launcher DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC_GAME})
source_group("Public Header Files" FILES ${SRC_PUBLIC})
ENDIF()

add_dependencies(launcher server_dll client_dll)
