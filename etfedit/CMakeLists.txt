cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	main.cpp
	
	stdafx.cpp
	stdafx.h
)

set(SRC_NOPCH
)

set(SRC_PUBLIC
)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

vs_pch(stdafx)

add_executable(etfedit
	${SRC}
	${SRC_PUBLIC}
	${SRC_NOPCH}
	CMakeLists.txt
)

builddir(etfedit)

target_link_libraries(etfedit SDL2main imgui graphics)
install(TARGETS etfedit DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()
