include_directories(${PROJECT_SOURCE_DIR}/MySTL)
set(APP_SRC test.cc)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(test ${APP_SRC})
