cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	main.cpp
	
	stdafx.cpp
	stdafx.h
)

vs_pch(stdafx)

add_executable(elf_test
	${SRC}
	CMakeLists.txt
)
builddir(elf_test)
target_link_libraries(elf_test ifsys)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()

add_custom_command(TARGET elf_test
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:elf> $<TARGET_FILE_DIR:elf_test>
)
