# CMake version
cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds are not supported. Please choose a different binary directory.")
	return()
endif()

# Only 64 bit is supported
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(FATAL_ERROR "32 bit isn't supported")
    return()
endif()

# Determine if engine is built as a subproject (using add_subdirectory)
# or if it is the master project.
if (NOT DEFINED LIB_MASTER_PROJECT)
  set(LIB_MASTER_PROJECT OFF)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(LIB_MASTER_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
  endif ()
endif ()

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configure CMake global variables
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_C_STANDARD 99)
set (CMAKE_INSTALL_MESSAGE LAZY)

set (CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
set (CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for OS X")

# Build version
set (LIB_VERSION_MAJOR 1)
set (LIB_VERSION_MINOR 0)
set (LIB_VERSION_PATCH 0)
set (LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

# Project declaration
project (native_libs
    VERSION ${LIB_VERSION}
    DESCRIPTION "Collection of C/C++ native libraries"
    HOMEPAGE_URL "https://github.com/amerkoleci/native_libs"
    LANGUAGES CXX C
)

# Configure CMake global variables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Build Configurations
if(NOT CMAKE_CONFIGURATION_TYPES)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()

# Build Type
if(NOT CMAKE_BUILD_TYPE)
	# By default, we use Release
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

include(GNUInstallDirs)

# Setting Platform
if(EMSCRIPTEN)
    set(LIB_PLATFORM_NAME "Web")
elseif(ANDROID)
    set(LIB_PLATFORM_NAME "Android")
elseif(IOS)
    set(LIB_PLATFORM_NAME "iOS")
elseif(APPLE)
    set(LIB_PLATFORM_NAME "macOS")
elseif(WINDOWS_STORE)
    set(LIB_PLATFORM_NAME "UWP")
elseif(WIN32)
    set(LIB_PLATFORM_NAME "Win32")
elseif(UNIX)
    set(LIB_PLATFORM_NAME "Unix")
    set(UNIX_WSI_SELECTION "XCB" CACHE STRING "Select WSI target (XCB, XLIB, WAYLAND, D2D)")
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Setting Architecture
if (DEFINED VCPKG_TARGET_ARCHITECTURE)
    set(LIB_ARCH ${VCPKG_TARGET_ARCHITECTURE})
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Ww][Ii][Nn]32$")
    set(LIB_ARCH x86)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Xx]64$")
    set(LIB_ARCH x64)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]$")
    set(LIB_ARCH arm)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]64$")
    set(LIB_ARCH arm64)
endif()

execute_process(COMMAND git rev-list --count HEAD
    OUTPUT_VARIABLE LIB_REVISION_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if ("${LIB_REVISION_ID}" STREQUAL "")
    # We don't have any source control so just hard-code all of this information to empty
    set (LIB_REVISION_ID 0)
    set (LIB_SHORT_CHANGE_SET 0)
    set (LIB_CHANGE_SET 0)
    set (LIB_CHANGE_SET_DATE "")
else ()
    execute_process(COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE LIB_SHORT_CHANGE_SET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND git rev-parse HEAD
        OUTPUT_VARIABLE LIB_CHANGE_SET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND git log -1 --abbrev=12 --date=format:%Y-%m-%d --pretty=format:%cd
        OUTPUT_VARIABLE LIB_CHANGE_SET_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

set(LIB_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
mark_as_advanced(LIB_ROOT_DIR)

# Platform Flags
if(EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
elseif(ANDROID)
elseif(UNIX)
    if(APPLE)
	    set(CMAKE_MACOSX_RPATH ON)
    else()
        set(CMAKE_SKIP_RPATH OFF)
    endif()
elseif(WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

# Options
if (IOS OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared library on (IOS, WEB)" FORCE)
elseif(ANDROID)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Always enable shared library on (ANDROID)" FORCE)    
else()
    option(BUILD_SHARED_LIBS "Build shared library" ON)
endif ()

# Options
option(LIB_INSTALL "Generate the install target" ${LIB_MASTER_PROJECT})

option(ALIMER_VULKAN "Make Vulkan backend available" ON)
option(ALIMER_WEBGPU "Make WebGPU backend available" ON)
if (WIN32 OR WINDOWS_STORE)
	option(ALIMER_D3D12 "Make D3D12 backend available" ON)
endif()

# Print current build configuration
message(STATUS "Engine version: ${LIB_VERSION}")
message(STATUS "GIT Revision ID: ${LIB_REVISION_ID}")
message(STATUS "GIT Short ChangeSet: ${LIB_SHORT_CHANGE_SET}")
message(STATUS "GIT ChangeSet: ${LIB_CHANGE_SET}")
message(STATUS "GIT ChangeSet Date: ${LIB_CHANGE_SET_DATE}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

message(STATUS "Detected platform: ${LIB_PLATFORM_NAME}")
message(STATUS "Detected architecture: ${LIB_ARCH}")

if (BUILD_SHARED_LIBS)
    message(STATUS "  Library         SHARED")
else ()
    message(STATUS "  Library         STATIC")
endif ()

if(ALIMER_VULKAN)
    message(STATUS "  Vulkan          ON")
endif()

if(ALIMER_D3D12)
    message(STATUS "  D3D12           ON")
endif()


if(UNIX AND NOT APPLE)
    message(STATUS "      - WSI:         ${UNIX_WSI_SELECTION}")
endif()

# src
add_subdirectory(src)

# Install README.md and license
if (LIB_INSTALL)
    install (FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
        DESTINATION .
    )
endif ()
