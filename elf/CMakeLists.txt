set(SRC
	stdafx.h
	stdafx.cpp

	elf.cpp
	mapeditor.cpp
	mapeditor.h

	gui_objects.cpp
	gui_objects.h

	input.cpp
	input.h
)

set(SRC_PUBLIC
	../public/include/elf/elf.h
	../public/include/elf/imapeditor.h
)

vs_pch(stdafx)

set(CMAKE_CXX_STANDARD 17)

add_library(elf MODULE
	${SRC}
	${SRC_PUBLIC}
	CMakeLists.txt
)

builddir(elf)

IF(UNIX AND NOT APPLE)
target_link_libraries(elf dl pthread ifsys graphics imgui)
ELSE()
target_link_libraries(elf ifsys graphics imgui)
ENDIF()

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
source_group("Public Headers" FILES ${SRC_PUBLIC})
ENDIF()

vs_link_no(elf)

add_custom_command(TARGET elf POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:elf> ${CMAKE_BINARY_DIR}/launcher/bin/)


