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

# ktx
CPMAddPackage(
    NAME ktx
    GITHUB_REPOSITORY KhronosGroup/KTX-Software
    GIT_TAG main
    GIT_SHALLOW ON
    GIT_MODULES ""
    OPTIONS
        "KTX_VERSION_FULL v4.3.2"
        "KTX_FEATURE_JNI OFF"
        "KTX_FEATURE_PY OFF"
        "KTX_FEATURE_TESTS OFF"
        "KTX_FEATURE_TOOLS_CTS OFF"
        "KTX_FEATURE_LOADTEST_APPS OFF"
        "KTX_FEATURE_DOC OFF"
        "KTX_FEATURE_VK_UPLOAD OFF"
        "KTX_FEATURE_GL_UPLOAD OFF"
        "KTX_FEATURE_TOOLS OFF"
)

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
