include(FetchContent)

# Third party
add_subdirectory(third_party)

# Alimer native library
add_subdirectory(alimer_native)

# Alimer GPU
add_subdirectory(alimer_gpu)

# cimgui
add_subdirectory(cimgui)

# cimnodes
add_subdirectory(cimnodes)

# spirv-reflect
add_subdirectory(spirv-reflect)

# spirv-cross
add_subdirectory(spirv-cross)

# vma
add_subdirectory(vma)

# GLFW
if(NOT (ANDROID OR EMSCRIPTEN OR WINDOWS_STORE))
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
    set(GLFW_INSTALL OFF CACHE BOOL "")

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3eaf1255b29fdf5c2895856c7be7d7185ef2b241
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(glfw)

    install(TARGETS glfw
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()
