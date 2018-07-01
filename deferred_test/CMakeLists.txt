cmake_minimum_required(VERSION 3.2.3)

vs_pch(stdafx)

# Add source files here

set(SRC_CORE
	stdafx.cpp
	stdafx.h
	
	main.cpp

)

add_executable(deferred_test
	${SRC_CORE}
	CMakeLists.txt
)
builddir(deferred_test)
vs_link_static(deferred_test)

SET(GAME_LIBRARIES SDL2 SDL2_image SDL2_ttf imgui graphics efs)

IF(UNIX AND NOT APPLE)
target_link_libraries(deferred_test ${GAME_LIBRARIES} dl pthread)
ELSE()
target_link_libraries(deferred_test ${GAME_LIBRARIES})
ENDIF()
install(TARGETS deferred_test DESTINATION bin/)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC_GAME})
ENDIF()
