include(FetchContent)

# Third party
add_subdirectory(third_party)

add_subdirectory(alimer_image)

# Alimer native library
add_subdirectory(alimer_native)

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
