cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	main.cpp
	
	stdafx.cpp
	stdafx.h

	window_filedialog.cpp
	window_filedialog.h
	window_model.cpp
	window_model.h
	window_properties.cpp
	window_properties.h
)

set(SRC_NOPCH
)

set(SRC_PUBLIC
	../public/src/emf_loader.cpp
	../public/include/emf_loader.h
	../public/include/gfx/window.h
)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

vs_pch(stdafx)

add_executable(mdlviewer
	${SRC}
	${SRC_PUBLIC}
	${SRC_NOPCH}
	CMakeLists.txt
)

target_link_libraries(mdlviewer SDL2main imgui graphics)
install(TARGETS mdlviewer DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()
