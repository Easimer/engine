cmake_minimum_required(VERSION 3.2.3)

# Add source files here

set(SRC
	imgui.cpp
	imgui_draw.cpp

	stdafx.cpp
	stdafx.h
	
	../public/include/gui/imgui.h
	../public/include/gui/imconfig.h
	../public/include/gui/imgui_internal.h
	../public/include/gui/stb_rect_pack.h
	../public/include/gui/stb_textedit.h
	../public/include/gui/stb_truetype.h

	../public/include/gui/LICENSE.txt
)

vs_pch(stdafx)

set(CMAKE_CXX_STANDARD 17)

add_library(imgui STATIC
	${SRC}
	CMakeLists.txt
)

IF(WIN32)
# Create filters
source_group("Source Files" FILES ${SRC})
ENDIF()

vs_link_no(imgui)
