cmake_minimum_required(VERSION 3.2.3)

IF(WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /Zi")
ENDIF()

# Add source files here

set(SRC
	smd_parser.cpp
	smd_parser.h
	
	main.cpp
	
	stdafx.cpp
	stdafx.h
)

vs_pch(stdafx)

set(CMAKE_CXX_STANDARD 17)

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
