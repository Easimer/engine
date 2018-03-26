set(SRC
	stdafx.cpp
	stdafx.h

	object.cpp

	aabb.cpp
	bounding_sphere.cpp
	plane.cpp
)

set(SRC_PUBLIC
	../public/include/phys/phys.h
	../public/include/phys/vector.h
	../public/include/phys/object.h

	../public/include/phys/aabb.h
	../public/include/phys/bounding_sphere.h
	../public/include/phys/plane.h
)

vs_pch(stdafx)

set(CMAKE_CXX_STANDARD 17)

add_library(phys STATIC
	${SRC}
	${SRC_PUBLIC}
	CMakeLists.txt
)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
source_group("Public Headers" FILES ${SRC_PUBLIC})
ENDIF()

vs_link_no(phys)
