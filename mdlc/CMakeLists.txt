cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	main.cpp

	emf_writer.cpp
	emf_writer.h
	
	stdafx.cpp
	stdafx.h
)

set(SRC_PUBLIC
	../public/src/qc_parser.cpp
	../public/include/qc_parser.h

	../public/include/emf.h
)

vs_pch(stdafx)

add_executable(mdlc
	${SRC}
	${SRC_PUBLIC}
	CMakeLists.txt
)

target_link_libraries(mdlc enl graphics)
install(TARGETS mdlc DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()
