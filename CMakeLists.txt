cmake_minimum_required(VERSION 3.31)

# Set the project
project(FUP VERSION 0.0.0.1)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)


if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Fetch zlib
FetchContent_Declare(
  zlib
  URL https://zlib.net/zlib-1.3.1.tar.gz
)
FetchContent_MakeAvailable(zlib)

# Collect public header files for installation
file(GLOB_RECURSE PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/include/*.hpp")

# Collect source files
file(GLOB_RECURSE CLIENT_SOURCES "${PROJECT_SOURCE_DIR}/src/client/*.cpp")
file(GLOB_RECURSE COMMON_SOURCES "${PROJECT_SOURCE_DIR}/src/common/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "${PROJECT_SOURCE_DIR}/src/server/*.cpp")

# Add the shared library
add_library(FUP SHARED 
    ${CLIENT_SOURCES}
    ${COMMON_SOURCES}
    ${SERVER_SOURCES}
)

target_include_directories(FUP
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(FUP PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

enable_testing()

add_executable(
  FUP_tests
  tests/test_client.cpp
)

target_link_libraries(FUP PUBLIC zlib)

target_link_libraries(FUP_tests
    PRIVATE
        FUP
        GTest::gtest_main
        GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(FUP_tests)

install(TARGETS FUP
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)