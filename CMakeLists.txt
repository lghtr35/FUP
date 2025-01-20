cmake_minimum_required(VERSION 3.31)

# Set the project
project(FUP VERSION 0.0.0.1)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Set dir
include_directories(${PROJECT_SOURCE_DIR})

# Add an executable
add_library(FUP SHARED src/client.cpp src/server.cpp src/connection.cpp src/message.cpp)

enable_testing()

add_executable(
  FUP_tests
  tests/test_client.cpp
)
target_link_libraries(
    FUP_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(FUP_tests)