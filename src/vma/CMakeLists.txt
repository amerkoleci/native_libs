# Define target name
set (TARGET_NAME vma)

set(SOURCE_FILES
    include/vk_mem_alloc.h
    src/vk_mem_alloc.cpp
)

add_library(${TARGET_NAME} ${SOURCE_FILES})
target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME}>
)
target_link_libraries(${TARGET_NAME} PRIVATE vulkan)

if (MSVC)
    # Set warning level 3
    target_compile_options(${TARGET_NAME} PRIVATE /W3)

    # Warnings as Errors Enabled
    target_compile_options(${TARGET_NAME} PRIVATE /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic)
endif ()

install (FILES "include/vk_mem_alloc.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME})

install(TARGETS ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
