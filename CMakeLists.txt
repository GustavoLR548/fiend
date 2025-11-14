cmake_minimum_required(VERSION 2.6)

project(Fiend)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/release)

if (WIN32)
   ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
endif (WIN32)

# Set debug flags only for Debug builds
# Release builds will use default optimizations (-O2 or -O3)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    message(STATUS "Building in Debug mode with -O0")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    # Add debug symbols even in release for better crash reports
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    message(STATUS "Building in Release mode with optimizations")
else()
    # Default to debug mode if not specified
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    message(STATUS "Building in unspecified mode (defaulting to Debug)")
endif()

# Make FMOD optional
FIND_PACKAGE(Fmod)

IF(FMOD_FOUND)
   INCLUDE_DIRECTORIES(${FMOD_INCLUDE_DIR})
   ADD_DEFINITIONS(-DUSE_FMOD)
   message(STATUS "Building with FMOD support")
ELSE()
   message(STATUS "FMOD not found - building without sound support")
ENDIF(FMOD_FOUND)

FIND_PACKAGE(Allegro REQUIRED)

IF(ALLEGRO_FOUND)
   INCLUDE_DIRECTORIES(${ALLEGRO_INCLUDE_DIR})
ENDIF(ALLEGRO_FOUND)

add_subdirectory(src/fiend)
add_subdirectory(src/mapeditor)
