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
GLint normal_offset_location = 10;
GLint snow_fill_rate_location = 11;
}

namespace config {
const int unitsPerMeter = 10;
const float verticesPerUnit = 0.003*unitsPerMeter;
const float verticesPerMeter = unitsPerMeter * verticesPerUnit;
const glm::vec2 terrainSizeM = { 700, 700 };
const glm::vec2 terrainSizeU = { terrainSizeM.x * unitsPerMeter, terrainSizeM.y
		* unitsPerMeter };
const int terrainVerticesX = terrainSizeM.x * verticesPerMeter;
const int terrainVerticesZ = terrainSizeM.y * verticesPerMeter;
const float sphereMinHeightU = 0.3 * unitsPerMeter;
const float sphereMaxHeightU = 3 * unitsPerMeter;
const int initSpheresAltitudeU = 100 * unitsPerMeter;
float verticalScaleU = 60 * unitsPerMeter;
float pixelResolutionU = 0.05*unitsPerMeter; //deformation texture - pixels per unit, 0.05
float snow_heightU = 2 * unitsPerMeter;
glm::vec3 gravityU = { 0, -10 * unitsPerMeter, 0 };
float normalOffsetU = 0.5*unitsPerMeter;
float snowFillRateEdgeU = snow_heightU*2; // units of snow per second

glm::uvec2 deformationTextureSize = {1024,1024};

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
