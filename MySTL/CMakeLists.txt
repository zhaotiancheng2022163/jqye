cmake_minimum_required(VERSION 2.8)

project(MySTL)

set(CMAKE_CXX_COMPILER clang++)
add_compile_options(-g -Wall -O0) #  -O2)
#add_definitions("-Wall -g")

# version
set(MySTL_VERSION_MAJOR 2)
set(MySTL_VERSION_MINOR 0)
set(MySTL_VERSION_PATCH 0)
set(MySTL_VERSION "${MySTL_VERSION_MAJOR}.${MySTL_VERSION_MINOR}.${MySTL_VERSION_PATCH}")
message(STATUS "The version of this project is: ${MySTL_VERSION}")

# build type
# set(CMAKE_BUILD_TYPE debug)

message(STATUS "The cmake_cxx_flags is: ${CMAKE_CXX_FLAGS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/Test)
