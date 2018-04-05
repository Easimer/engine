cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	main.cpp
	
	stdafx.cpp
	stdafx.h

	window_properties.cpp
	window_properties.h
	window_preview.cpp
	window_preview.h
)

set(SRC_NOPCH
)

set(SRC_PUBLIC
	../public/include/gfx/window.h

	../public/include/qc.h
)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

vs_pch(stdafx)

add_executable(matedit
	${SRC}
	${SRC_PUBLIC}
	${SRC_NOPCH}
	CMakeLists.txt
)

target_link_libraries(matedit SDL2main imgui graphics)
install(TARGETS matedit DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()
