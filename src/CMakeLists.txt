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
set(SDL_TEST_ENABLED_BY_DEFAULT OFF)
set(SDL_SHARED_ENABLED_BY_DEFAULT ON)
set(SDL_STATIC_ENABLED_BY_DEFAULT OFF)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG 9667aa18e6c29885d8cd45efc2cf8bcb97270d7d
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL3)

install(TARGETS SDL3-shared
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
