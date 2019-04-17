#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <glm/glm.hpp>

namespace globals {

int screenWidth = 1600;
int screenHeight = 900;

GLint mvp_location = 0;
GLint terrain_size_location = 2;
GLint vertical_scaling_location = 3;
GLint camera_position_location = 4;
GLint gravity_location = 5;
GLint time_delta_location = 6;
GLint pixel_resolution_location = 8;
GLint snow_height_location = 9;
}

namespace config {
bool geometryMode = false;
const float VERTEX_PER_METER = 0.05;
const glm::vec2 TERRAIN_SIZE_M = { 1000, 1000 };
const int TERRAIN_X = TERRAIN_SIZE_M.x * VERTEX_PER_METER;
const int TERRAIN_Z = TERRAIN_SIZE_M.y * VERTEX_PER_METER;
const int SPHERES_INIT_HEIGHT = config::TERRAIN_SIZE_M.x / 5;
const int MAX_SPHERE_SIZE = 15;
float vertical_scaling = 150;
float pixel_resolution = 10; //pixels per meter
float snow_height = 3;
glm::vec3 gravity = { 0, -10, 0 };
}

static bool check_opengl() {
	GLenum err = glGetError();
	if (err == GL_NO_ERROR)
		return true;
	std::cout << "OpenGL message: " << (int) err << std::endl;
	return false;
}

static void checkGl() {
	if (!check_opengl()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

#endif /* GLOBALS_H_ */
