cmake_minimum_required(VERSION 3.0)

project(enbine-client)

add_subdirectory(enbine)
include_directories(enbine/include)

add_executable(enbine-client src/main.cpp)

#THE LIBRARY
target_link_libraries(enbine-client enbine)

#some libs for platform specific things with graphics
target_link_libraries(enbine-client glfw GL)