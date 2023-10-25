cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

project(cimnodes)

set (CMAKE_CXX_STANDARD 11)

#general settings
file(GLOB IMNODES_SOURCES
    ../cimgui/imgui/imgui.cpp
    ../cimgui/imgui/imgui_draw.cpp
    ../cimgui/imgui/imgui_demo.cpp
    ../cimgui/imgui/imgui_widgets.cpp
	../cimgui/imgui/imgui_tables.cpp
    cimnodes.h
    cimnodes.cpp
    imnodes/imnodes.h
    imnodes/imnodes.cpp
)

set(IMNODES_STATIC "no" CACHE STRING "Build as a static library")

#add library and link
if (IMNODES_STATIC)
    add_library(cimnodes STATIC ${IMNODES_SOURCES})
else (IMNODES_STATIC)
    add_library(cimnodes SHARED ${IMNODES_SOURCES})
endif (IMNODES_STATIC)

#target_compile_definitions(cimgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1)
#if (WIN32)
#    target_compile_definitions(cimgui PUBLIC IMGUI_IMPL_API=extern\t\"C\"\t__declspec\(dllexport\))
#    list(APPEND IMGUI_LIBRARIES imm32)
#else (WIN32)
#    target_compile_definitions(cimgui PUBLIC IMGUI_IMPL_API=extern\t\"C\"\t)
#endif (WIN32)

target_include_directories(cimnodes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imnodes)
target_include_directories(cimnodes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../cimgui/imgui)
target_include_directories(cimnodes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../cimgui)
set_target_properties(cimnodes PROPERTIES PREFIX "")
target_compile_definitions(cimnodes PRIVATE IMNODES_NAMESPACE=imnodes)

#install
install(TARGETS cimnodes
    ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
)
