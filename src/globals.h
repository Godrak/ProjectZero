#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <glm/glm.hpp>

namespace globals {

glm::ivec2 screenResolution = {1600,900};

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
GLint veloctiy_limit_location = 12;
GLint screen_size_location = 13;
GLint light_params_location = 14;
GLint ambient_volume_index_location = 15;
}

namespace config {
const int unitsPerMeter = 30;
const float verticesPerMeter = unitsPerMeter * 0.01; //0.3
const glm::vec2 terrainSizeM = { 500, 500 };
const glm::vec2 terrainSizeU = { terrainSizeM.x * unitsPerMeter, terrainSizeM.y
		* unitsPerMeter };
const int terrainVerticesX = terrainSizeM.x * verticesPerMeter;
const int terrainVerticesZ = terrainSizeM.y * verticesPerMeter;
const float sphereMinHeightU = 1 * unitsPerMeter;
const float sphereMaxHeightU = 3 * unitsPerMeter;
const int initSpheresAltitudeU = 100 * unitsPerMeter;
float verticalScaleU = 30 * unitsPerMeter;
float pixelResolutionU = 3.5/unitsPerMeter; //deformation texture - pixels per unit, 0.05
float snow_heightU = 2 * unitsPerMeter;
glm::vec3 gravityU = { 0, -10 * unitsPerMeter, 0 };
float spheresVelocitiyLimit = 20*unitsPerMeter;
float normalOffsetU = terrainSizeM.x*unitsPerMeter/1000.0;
float snowFillRateEdgeU = snow_heightU*5; // units of snow per second

glm::uvec2 deformationTextureSize = {1024,1024};

glm::vec3 lightParams = {1.0, 0.015, 0.0009};//Kc, Kq, Kl

bool geometryMode = false;
bool defferedShading = false;
bool lightVolumes = false;
}

static bool check_opengl() {
	GLenum err = glGetError();
	if (err == GL_NO_ERROR)
		return true;
	std::cout << "OpenGL message: " << (int) err << std::endl;
	return false;
}

static bool checkGLCall(GLenum err) {
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
