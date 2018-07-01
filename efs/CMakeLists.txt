set(SRC
	stdafx.h
	stdafx.cpp

	efs.cpp
)

set(SRC_PUBLIC
	../public/include/efs/efs.h
)

set(CMAKE_CXX_STANDARD 17)

add_library(efs STATIC
	${SRC}
	${SRC_PUBLIC}
	CMakeLists.txt
)

add_executable(efs_tool
	efs_tool.cpp
	${SRC_PUBLIC}
	CMakeLists.txt
)

builddir(efs_tool)
target_link_libraries(efs_tool efs enl)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
source_group("Public Headers" FILES ${SRC_PUBLIC})
ENDIF()

vs_link_no(efs)

