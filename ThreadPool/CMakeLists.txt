cmake_minimum_required(VERSION 3.2)
project(ThreadPool CXX)

add_definitions(-DFUNCTION_TEST)
add_definitions(-DCLASS_TEST)
add_definitions(-DLAMBDA_TEST)
# set(CMAKE_CXX_COMPILER clang++)
# set(CMAKE_CXX_COMPILER g++)
add_compile_options(-g -Wall -O0) #  -O2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_BUILD_TYPE debug)
# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(ThreadPool ${PROJECT_SOURCE_DIR}/main.cc)

add_library(test STATIC ./src/test.cc)
add_library(threadpool STATIC ./src/ThreadPool.cc)
target_link_libraries(ThreadPool threadpool test)
