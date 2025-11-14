cmake_minimum_required(VERSION 3.10)

project(Fiend VERSION 1.0.0 LANGUAGES C CXX)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Module path for custom Find modules (keeping for backward compatibility)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

# Output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/release)

# Windows-specific definitions
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug or Release)" FORCE)
    message(STATUS "No build type specified, defaulting to Release")
endif()

# Compiler flags based on build type
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-g -O0 -Wall -Wextra)
    message(STATUS "Building in Debug mode: -g -O0 -Wall -Wextra")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    # Release with debug symbols for crash reports
    add_compile_options(-g -O2)
    add_definitions(-DNDEBUG)
    message(STATUS "Building in Release mode: -g -O2 -DNDEBUG")
endif()

# Find required packages
find_package(PkgConfig)

# Try to find Allegro using pkg-config first (modern approach)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(ALLEGRO allegro)
endif()

# Fall back to custom Find module if pkg-config didn't work
if(NOT ALLEGRO_FOUND)
    find_package(Allegro REQUIRED)
    if(ALLEGRO_FOUND)
        # Convert old-style variables to new-style for consistency
        set(ALLEGRO_INCLUDE_DIRS ${ALLEGRO_INCLUDE_DIR})
        set(ALLEGRO_LIBRARIES ${ALLEGRO_LIBRARY})
    endif()
endif()

if(ALLEGRO_FOUND)
    include_directories(${ALLEGRO_INCLUDE_DIRS})
    message(STATUS "Found Allegro: ${ALLEGRO_LIBRARIES}")
else()
    message(FATAL_ERROR "Allegro 4 library not found! Install with: sudo apt-get install liballegro4-dev")
endif()

# Add subdirectories
add_subdirectory(src/fiend)
add_subdirectory(src/mapeditor)

# Print build summary
message(STATUS "========================================")
message(STATUS "Fiend Build Configuration:")
message(STATUS "  Build Type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler:    ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  CXX Compiler:  ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Allegro:       Found")
message(STATUS "  Audio:         miniaudio (embedded)")
message(STATUS "  Output Dir:    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "========================================")
