set(SOURCE_FILES
    include/D3D12MemAlloc.h
    src/D3D12MemAlloc.cpp
    src/D3D12MemAlloc_Exports.cpp
)

add_library(D3D12MemoryAllocator ${SOURCE_FILES})
target_include_directories(D3D12MemoryAllocator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
)

set_target_properties(
    D3D12MemoryAllocator PROPERTIES

    CXX_EXTENSIONS OFF
    # Use C++14
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON

    OUTPUT_NAME "D3D12MA"
    # Postfix for different profiles
    DEBUG_POSTFIX "d"
    RELWITHDEBINFO_POSTFIX "rd"
    MINSIZEREL_POSTFIX "s"
)

if (LIB_INSTALL)
    install (FILES "include/D3D12MemAlloc.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/D3D12MA)

    install(TARGETS D3D12MemoryAllocator
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif ()
