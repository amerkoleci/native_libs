include(FetchContent)

# Third party
add_subdirectory(third_party)

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

if (WIN32 OR WINDOWS_STORE)
    add_subdirectory(D3D12MemoryAllocator)
endif ()
