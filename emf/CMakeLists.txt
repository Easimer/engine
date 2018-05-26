set(SRC
	stdafx.h
	stdafx.cpp

	emf.cpp
)

set(SRC_PUBLIC
	../public/include/emf/emf.h
)

vs_pch(stdafx)

set(CMAKE_CXX_STANDARD 17)

add_library(emf STATIC
	${SRC}
	${SRC_PUBLIC}
	CMakeLists.txt
)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
source_group("Public Headers" FILES ${SRC_PUBLIC})
ENDIF()

vs_link_no(emf)

