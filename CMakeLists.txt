cmake_minimum_required(VERSION 3.16)

project(skip_list LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

add_executable(testing_skip_list
    tests/test.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

target_include_directories(testing_skip_list PRIVATE
    include
)

enable_testing()

add_test(
    NAME testing_skip_list
    COMMAND ${CMAKE_COMMAND} -E env VERBOSE=1 ./testing_skip_list
)

set_tests_properties(testing_skip_list PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    LABELS "testing_skip_list"
)
