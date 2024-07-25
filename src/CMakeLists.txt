include(FetchContent)

# Third party
add_subdirectory(third_party)

# cimgui
add_subdirectory(cimgui)

# cimnodes
add_subdirectory(cimnodes)

# cimguizmo
add_subdirectory(cimguizmo)

# cimplot
add_subdirectory(cimplot)

# cgltf
add_subdirectory(cgltf)

# ufbx
add_subdirectory(ufbx)

# spirv-reflect
add_subdirectory(spirv-reflect)

# spirv-cross
add_subdirectory(spirv-cross)

# vma
add_subdirectory(vma)

# meshoptimizer
add_subdirectory(meshoptimizer)

if (WIN32 OR WINDOWS_STORE)
    add_subdirectory(D3D12MemoryAllocator)
endif ()

# glfw
if(NOT (EMSCRIPTEN OR ANDROID OR WINDOWS_STORE))
    CPMAddPackage(
        NAME GLFW
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.4
        GIT_SHALLOW ON
        OPTIONS
          "GLFW_BUILD_TESTS OFF"
          "GLFW_BUILD_EXAMPLES OFF"
          "GLFW_BUILD_DOCS OFF"
    )
endif ()
