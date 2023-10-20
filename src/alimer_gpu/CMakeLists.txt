# Define target name
set (TARGET_NAME alimer_gpu)

add_library(${TARGET_NAME}
    # Public Headers
    include/alimer_gpu.h
     # Internal Headers
    src/alimer_gpu_internal.h
    # Source files
    src/alimer_gpu.c
    src/alimer_gpu_vulkan.cpp
)
target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_GPU_IMPLEMENTATION)

if (BUILD_SHARED_LIBS)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_GPU_SHARED_LIBRARY)
endif ()

target_include_directories(${TARGET_NAME}
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

if(ALIMER_VULKAN)
    target_link_libraries(${TARGET_NAME} PRIVATE vulkan)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_VULKAN)
endif()

if(ALIMER_D3D12)
    target_sources(${TARGET_NAME} PRIVATE
        src/alimer_gpu_d3d12.cpp
        src/third_party/D3D12MemAlloc.h
        src/third_party/D3D12MemAlloc.cpp
    )

    target_link_libraries(${TARGET_NAME} PRIVATE DirectX-Headers)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_D3D12)
endif()

if (MSVC)
    # Set warning level 3
    target_compile_options(${TARGET_NAME} PRIVATE /W3)

    # Warnings as Errors Enabled
    target_compile_options(${TARGET_NAME} PRIVATE /WX)

    # Debug information
	target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:/Zi>)

    # Enable full optimization in dev/release
	target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:/Od> $<$<NOT:$<CONFIG:Debug>>:/Ox>)

    # Inline function expansion
	target_compile_options(${TARGET_NAME} PRIVATE /Ob2)

    # Enable intrinsic functions in dev/release
	target_compile_options(${TARGET_NAME} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/Oi>)

    # Favor fast code
	target_compile_options(${TARGET_NAME} PRIVATE /Ot)

    # Enable fiber-safe optimizations in dev/release
	target_compile_options(${TARGET_NAME} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/GT>)

	# Enable string pooling
	target_compile_options(${TARGET_NAME} PRIVATE /GF)

    # Use security checks only in debug
	target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:DEBUG>:/sdl> $<$<NOT:$<CONFIG:DEBUG>>:/sdl->)

	# Enable function-level linking
	target_compile_options(${TARGET_NAME} PRIVATE /Gy)

    # Use fast floating point model
    target_compile_options(${TARGET_NAME} PRIVATE /fp:fast "$<$<NOT:$<CONFIG:DEBUG>>:/guard:cf>")
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wunused-variable)
endif()

if(WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(${TARGET_NAME} PRIVATE NOMINMAX)
    target_compile_definitions(${TARGET_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
endif ()

if (LIB_INSTALL)
    install (FILES "include/alimer_gpu.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME})

    install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()
