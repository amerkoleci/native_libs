# Third party
add_subdirectory(third_party)

# image library
add_subdirectory(alimer_image)

# audio library
add_subdirectory(alimer_audio)

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
    #set(BUILD_SHARED_LIBS ON CACHE BOOL "")

    if (VGPU_WSI_SELECTION STREQUAL WAYLAND)
        set(GLFW_USE_WAYLAND ON CACHE BOOL "")
    endif()

    add_subdirectory(glfw)
endif()
