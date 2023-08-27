cmake_minimum_required(VERSION 3.12)
project(Lute_Base LANGUAGES CXX)

# Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wold-style-cast")

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set srcs
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cc include/*.h)
add_library(Lute_Base SHARED ${srcs})
target_link_libraries(Lute_Base PUBLIC pthread)
target_include_directories(Lute_Base PUBLIC include)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif()
