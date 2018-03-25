cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	main.cpp
	
	stdafx.cpp
	stdafx.h
)

vs_pch(stdafx)

add_executable(phys_testbed
	${SRC}
	CMakeLists.txt
)

target_link_libraries(phys_testbed phys)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()
