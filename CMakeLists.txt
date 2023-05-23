cmake_minimum_required(VERSION 3.12)
project(LuxUtils LANGUAGES CXX)

# Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cc include/*.h)
add_library(LuxUtils SHARED ${srcs})
target_include_directories(LuxUtils PUBLIC include)

add_subdirectory(test)
