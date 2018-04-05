set(SRC
	stdafx.h
	stdafx.cpp

	server.cpp
	server_handlers.cpp
	client.cpp
	client_handlers.cpp
)

set(SRC_NOPCH
)

set(SRC_PUBLIC
	../public/include/net/networking.h
	../public/include/net/server.h
	../public/include/net/client.h
)

vs_pch(stdafx)

IF(WIN32)
set_source_files_properties(${SRC_NOPCH}
									PROPERTIES COMPILE_FLAGS "/Y-")
ENDIF()

set(CMAKE_CXX_STANDARD 17)

add_library(networking STATIC
	${SRC}
	${SRC_NOPCH}
	${SRC_PUBLIC}
	CMakeLists.txt
)

target_link_libraries(graphics)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
source_group("Public Headers" FILES ${SRC_PUBLIC})
ENDIF()

vs_link_no(graphics)

