# Define target name
set (TARGET_NAME alimer_native)

add_library(${TARGET_NAME}
    # Public Headers
    include/alimer.h
     # Internal Headers
    src/alimer_internal.h
    # Source files
    src/alimer_memory.c
    src/alimer_audio.c
    src/alimer_font.c
)
target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_IMPLEMENTATION=1)

if (BUILD_SHARED_LIBS)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_SHARED_LIBRARY=1)
endif ()

target_include_directories(${TARGET_NAME}
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

if (LIB_INSTALL)
    install (FILES "include/alimer.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME})

    install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()
