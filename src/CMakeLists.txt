include(FetchContent)

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

# SDL3
set(SDL_SHARED ON CACHE BOOL "Don't build SDL as shared library")
set(SDL_STATIC OFF CACHE BOOL "Build a SDL static library")
set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests")
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Disable SDL test library")

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG 72034b9a07776e26cc2c83f519549c457c3452f7
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL3)

install(TARGETS SDL3-shared
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
