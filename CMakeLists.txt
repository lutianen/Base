cmake_minimum_required(VERSION 3.12)
project(LuxBase LANGUAGES CXX)

# Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wold-style-cast")

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cc include/*.h)
add_library(luxbase SHARED ${srcs})
target_include_directories(luxbase PUBLIC include)

add_subdirectory(test)
