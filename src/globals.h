#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <glm/glm.hpp>

namespace globals {

int screenWidth = 1600;
int screenHeight = 900;

GLint mvp_location = 0;
GLint heightmap_location = 1;
GLint terrain_size_location = 2;
GLint vertical_scaling_location = 3;
GLint camera_position_location = 4;
}

namespace config {
bool geometryMode = false;
const float VERTEX_PER_METER = 0.1;
const glm::vec2 TERRAIN_SIZE_M = { 513, 513 };
const int TERRAIN_X = TERRAIN_SIZE_M.x * VERTEX_PER_METER;
const int TERRAIN_Z = TERRAIN_SIZE_M.y * VERTEX_PER_METER;
const int TERRAIN_HALF_X = TERRAIN_X / 2 + 1;
const int TERRAIN_HALF_Z = TERRAIN_Z / 2 + 1;

float vertical_scaling = 60;
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
