cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	main.cpp
	
	stdafx.cpp
	stdafx.h
)

vs_pch(stdafx)

add_executable(smd_parser
	${SRC}
	CMakeLists.txt
)

target_link_libraries(smd_parser enl)
install(TARGETS smd_parser DESTINATION bin/tools/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()
