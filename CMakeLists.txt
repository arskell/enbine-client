cmake_minimum_required(VERSION 3.0)

project(enbine-client)


add_executable(enbine-client src/main.cpp)

target_link_libraries(enbine-client glfw GL)