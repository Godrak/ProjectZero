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
const int unitsPerMeter = 10;
const float verticesPerUnit = 0.001*unitsPerMeter;
const float verticesPerMeter = unitsPerMeter * verticesPerUnit;
const glm::vec2 terrainSizeM = { 800, 800 };
const glm::vec2 terrainSizeU = { terrainSizeM.x * unitsPerMeter, terrainSizeM.y
		* unitsPerMeter };
const int terrainVerticesX = terrainSizeM.x * verticesPerMeter;
const int terrainVerticesZ = terrainSizeM.y * verticesPerMeter;
const float sphereMinHeightU = 0.5 * unitsPerMeter;
const float sphereMaxHeightU = 2 * unitsPerMeter;
const int initSpheresAltitudeU = 100 * unitsPerMeter;
float verticalScaleU = 50 * unitsPerMeter;
float pixelResolutionU = 0.05 * unitsPerMeter; //deformation texture - pixels per unit
float snow_heightU = 2 * unitsPerMeter;
glm::vec3 gravityU = { 0, -10 * unitsPerMeter, 0 };


bool geometryMode = false;
bool defferedShading = false;
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
