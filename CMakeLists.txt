cmake_minimum_required(VERSION 3.1)

# Set the project name
project(FUP)

# Add an executable
add_library(FUP src/fup/client.cpp src/fup/server.cpp src/fup/connection.cpp src/fup/message.cpp)
add_executable(FUP_test main.cpp)
target_link_libraries(FUP_test FUP)