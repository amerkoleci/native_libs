cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

project(cimguizmo)

set (CMAKE_CXX_STANDARD 11)

#general settings
file(GLOB IMGUIZMO_SOURCES
    ../cimgui/imgui/imgui.cpp
    ../cimgui/imgui/imgui_draw.cpp
    ../cimgui/imgui/imgui_demo.cpp
    ../cimgui/imgui/imgui_widgets.cpp
	../cimgui/imgui/imgui_tables.cpp
    cimguizmo.h
    cimguizmo.cpp
    ImGuizmo/ImGuizmo.h
    ImGuizmo/ImGuizmo.cpp
)

set(IMGUIZMO_STATIC "no" CACHE STRING "Build as a static library")

#add library and link
if (IMGUIZMO_STATIC)
    add_library(cimguizmo STATIC ${IMGUIZMO_SOURCES})
else (IMGUIZMO_STATIC)
    add_library(cimguizmo SHARED ${IMGUIZMO_SOURCES})
endif (IMGUIZMO_STATIC)

target_include_directories(cimguizmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo)
target_include_directories(cimguizmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../cimgui/imgui)
target_include_directories(cimguizmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../cimgui)
set_target_properties(cimguizmo PROPERTIES PREFIX "")

#install
install(TARGETS cimguizmo
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
