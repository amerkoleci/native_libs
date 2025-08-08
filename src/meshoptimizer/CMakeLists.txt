# Define target name
set (TARGET_NAME meshoptimizer)

file (GLOB HEADER_FILES src/*.h)
file (GLOB SOURCE_FILES src/*.cpp)

add_library(${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})

if(BUILD_SHARED_LIBS)
    set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${TARGET_NAME} PROPERTIES VISIBILITY_INLINES_HIDDEN ON)

    # soversion may be requested via -DMESHOPT_SOVERSION=n; note that experimental APIs (marked with MESHOPTIMIZER_EXPERIMENTAL) are not ABI-stable
    if(MESHOPT_SOVERSION)
        set_target_properties(${TARGET_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${MESHOPT_SOVERSION})
    endif()

    if(WIN32)
        target_compile_definitions(${TARGET_NAME} INTERFACE "MESHOPTIMIZER_API=__declspec(dllimport)")
        target_compile_definitions(${TARGET_NAME} PRIVATE "MESHOPTIMIZER_API=__declspec(dllexport)")
    else()
        target_compile_definitions(${TARGET_NAME} PUBLIC "MESHOPTIMIZER_API=__attribute__((visibility(\"default\")))")
    endif()
endif()

install (FILES "src/meshoptimizer.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME})

install(TARGETS ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
