# Define target name
set (TARGET_NAME spirv-reflect)

set(SOURCE_FILES
    spirv_reflect.h
    spirv_reflect.c
)

add_library(${TARGET_NAME} ${SOURCE_FILES})
target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME}>
)
target_link_libraries(${TARGET_NAME} PRIVATE vulkan)

if (LIB_INSTALL)
    install (FILES "spirv_reflect.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME})

    install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()
