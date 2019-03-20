project(ProjectZero)

set(CMAKE_CXX_STANDARD 17)

cmake_minimum_required(VERSION 3.8.4)

find_package(glm REQUIRED)

add_executable(Zero zero.cpp camera.h globals.h shaders.h terrain.h misc/heightmap.c)
add_custom_target(Shaders ALL)
add_custom_command(TARGET Shaders PRE_BUILD COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/*.glsl ${CMAKE_BINARY_DIR} | :  )
target_link_libraries(Zero glfw epoxy glm)
