set(SRC
	stdafx.h
	stdafx.cpp
)

if(WIN32)
	set(SRC_PLAT
		fileio_win32.cpp
		endian_win32.cpp
	)
elseif(UNIX)
	set(SRC_PLAT
		fileio_linux.cpp
		endian_linux.cpp
	)
endif()

set(SRC_PUBLIC
	../public/include/plat/fileio.h
)

vs_pch(stdafx)

set(CMAKE_CXX_STANDARD 17)

add_library(platlayer STATIC
	${SRC}
	${SRC_PLAT}
	${SRC_PUBLIC}
	CMakeLists.txt
)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
source_group("Public Headers" FILES ${SRC_PUBLIC})
ENDIF()

vs_link_no(platlayer)

