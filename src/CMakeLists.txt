project(ProjectZero)

set(CMAKE_CXX_STANDARD 17)

cmake_minimum_required(VERSION 3.8.4)

find_package(glm REQUIRED)

add_executable(Zero zero.cpp camera.h globals.h shaders.h terrain.h spheres.h misc/heightmap.c)
add_custom_target(TerrainShaders ALL)
add_custom_command(TARGET TerrainShaders PRE_BUILD COMMAND 
	cp -r ${CMAKE_CURRENT_SOURCE_DIR}/terrain_shaders ${CMAKE_BINARY_DIR} | :  )
target_link_libraries(Zero glfw epoxy glm)
