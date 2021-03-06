#include "epoxy/gl.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <iostream>

#include "globals.h"
#include "camera.h"
#include "shaders.h"
#include "terrain.h"
#include "spheres.h"
#include "snow.h"
#include "skybox.h"
#include "deffered_render.h"

//#define DEBUG

namespace fps {
static double fpsOrigin = 0.0;
static double fpsLastLog = 0.0;
static int frameCounter = 0;
const int FPS_MASK = 15;
const double EXP_FACTOR = 0.5;
const double FPS_MEASURE_INTERVAL = 0.5;
const double FPS_LOG_INTERVAL = 2.0;
static double fpsFps = 30.0;

static void fps_start() {
	fpsOrigin = fpsLastLog = glfwGetTime();
	fpsFps = 60.0;
	frameCounter = 0;
}

static void fps_frame(bool log = false) {
	if (!(++frameCounter & FPS_MASK))
		return;

	double current = glfwGetTime();
	if (current < fpsOrigin + FPS_MEASURE_INTERVAL)
		return;

	double currentFps = frameCounter / (current - fpsOrigin);
	fpsFps = fpsFps * EXP_FACTOR + currentFps * (1.0 - EXP_FACTOR);
	frameCounter = 0;
	fpsOrigin = current;

	if (log && current > fpsLastLog + FPS_LOG_INTERVAL) {
		std::cout << current << ": FPS: " << fpsFps << std::endl;
		fpsLastLog = current;
	}
}

}

namespace glfwContext {

double last_xpos, last_ypos;
GLFWwindow *window;
int vsync = 1;
char forth_back = ' ';
char left_right = ' ';
char up_down = ' ';

static void error_callback(int error, const char *description) {
	std::cout << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_F2:
			config::geometryMode = 1 - config::geometryMode;
			break;
		case GLFW_KEY_F3:
			vsync = 1 - vsync;
			std::cout << "vsync: " << vsync << std::endl;
			glfwSwapInterval(vsync);
			break;
		case GLFW_KEY_R:
			config::updateCameraPosition = 1 - config::updateCameraPosition;
			break;
		case GLFW_KEY_W:
			forth_back = ('w');
			break;

		case GLFW_KEY_S:
			forth_back = ('s');
			break;

		case GLFW_KEY_A:
			left_right = ('a');
			break;

		case GLFW_KEY_D:
			left_right = ('d');
			break;

		case GLFW_KEY_Q:
			up_down = ('q');
			break;

		case GLFW_KEY_E:
			up_down = ('e');
			break;
		case GLFW_KEY_KP_ADD:
			camera::stepSize += 2 / 60.0f;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			camera::stepSize -= 2 / 60.0f;
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_W:
			case GLFW_KEY_S:
			forth_back = ' ';
			break;
		case GLFW_KEY_A:
			case GLFW_KEY_D:
			left_right = ' ';
			break;
		case GLFW_KEY_Q:
			case GLFW_KEY_E:
			up_down = ' ';
			break;
		}
	}
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
	glm::vec2 offset;
	offset[0] = -xpos + last_xpos;
	offset[1] = ypos - last_ypos;
	offset *= 0.001 * camera::rotationSpeed;
	camera::moveCamera(offset);

	last_xpos = xpos;
	last_ypos = ypos;
}

static void initGlfw() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(globals::screenResolution.x, globals::screenResolution.y, "ProjectZero", NULL, NULL);
	if (!window)
		exit(-1);

	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(vsync);

}

}

namespace rendering {

float lastTime;
float currentTime;
glm::vec3 lastCameraPosition = camera::position;

void switchConfiguration() {
	if (config::geometryMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void render() {
	currentTime = float(glfwGetTime());
	auto delta = currentTime - lastTime;
	lastTime = currentTime;
	glfwGetFramebufferSize(glfwContext::window, &globals::screenResolution.x, &globals::screenResolution.y);

	glViewport(0, 0, globals::screenResolution.x, globals::screenResolution.y);

	camera::moveCamera(glfwContext::forth_back);
	camera::moveCamera(glfwContext::up_down);
	camera::moveCamera(glfwContext::left_right);

	if (config::updateCameraPosition)
		lastCameraPosition = camera::position;

	glm::mat4x4 model_view_projection = glm::mat4x4();
	camera::applyViewTransform(model_view_projection);
	camera::applyProjectionTransform(model_view_projection);

	if (config::defferedShading) {
		glBindFramebuffer(GL_FRAMEBUFFER, deffered_render::gBuffer);
		glDisable(GL_BLEND);
		glDepthMask(true);
		glDisable(GL_CULL_FACE);

	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//DEFORMATION AND PHYSICS COMPUTE SHADER DISPATCH
#ifdef DEBUG
	checkGl();
	std::cout << "deformation and physics" << std::endl;
#endif
	glUseProgram(shaderProgram::spheres_compute_program);
	glBindVertexArray(spheres::spheresVAO);

	glUniform2f(globals::terrain_size_location, config::terrainSizeU.x, config::terrainSizeU.y);
	glUniform1f(globals::vertical_scaling_location, config::verticalScaleU);
	glUniform3fv(globals::camera_position_location, 1, glm::value_ptr(lastCameraPosition));
	glUniform3fv(globals::gravity_location, 1, glm::value_ptr(config::gravityU));
	glUniform1f(globals::time_delta_location, delta);
	glUniform1f(globals::pixel_resolution_location, config::pixelResolutionU);
	glUniform1f(globals::veloctiy_limit_location, config::spheresVelocitiyLimit);

	glDispatchCompute(spheres::instanceCount, 1, 1);

	glUseProgram(0);
	glBindVertexArray(0);

	//SNOW FILL DISPATCH
#ifdef DEBUG
	checkGl();
	std::cout << "snow fill" << std::endl;
#endif
	glUseProgram(shaderProgram::snow_fill_program);
	glUniform1f(globals::snow_fill_rate_location, config::snowFillRateEdgeU);
	glUniform1f(globals::time_delta_location, delta);
	glUniform1f(globals::pixel_resolution_location, config::pixelResolutionU);
	glUniform3fv(globals::camera_position_location, 1, glm::value_ptr(lastCameraPosition));

	glDispatchCompute(config::deformationTextureSize.x / 32, config::deformationTextureSize.y / 32, 1);

	glUseProgram(0);

	//TERRAIN DRAW
#ifdef DEBUG
	checkGl();
	std::cout << "terrain draw" << std::endl;
#endif
	glUseProgram(shaderProgram::terrain_program);
	glBindVertexArray(terrain::terrainVAO);

	glUniformMatrix4fv(globals::mvp_location, 1, GL_FALSE, glm::value_ptr(model_view_projection));

	glUniform2f(globals::terrain_size_location, config::terrainSizeU.x, config::terrainSizeU.y);
	glUniform1f(globals::vertical_scaling_location, config::verticalScaleU);
	glUniform3fv(globals::camera_position_location, 1, glm::value_ptr(lastCameraPosition));
	glUniform1f(globals::normal_offset_location, config::normalOffsetU);

	glUniform1f(globals::snow_height_displacement_size_location, config::snow_height_displacement_size);
	glUniform1f(globals::snow_texture_size_location, config::snow_texture_size);

	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, terrain::indicesData.size(), GL_UNSIGNED_INT, 0);
	glUseProgram(0);
	glBindVertexArray(0);

	//SNOW DRAW
#ifdef DEBUG
	checkGl();
	std::cout << "snow draw" << std::endl;
#endif
	glUseProgram(shaderProgram::snow_program);
	glBindVertexArray(snow::snowVAO);

	glUniformMatrix4fv(globals::mvp_location, 1, GL_FALSE, glm::value_ptr(model_view_projection));

	glUniform2f(globals::terrain_size_location, config::terrainSizeU.x, config::terrainSizeU.y);
	glUniform1f(globals::vertical_scaling_location, config::verticalScaleU);
	glUniform3fv(globals::camera_position_location, 1, glm::value_ptr(lastCameraPosition));
	glUniform1f(globals::snow_height_location, config::snow_heightU);
	glUniform1f(globals::pixel_resolution_location, config::pixelResolutionU);
	glUniform1f(globals::normal_offset_location, config::normalOffsetU);

	glUniform1f(globals::snow_height_displacement_size_location, config::snow_height_displacement_size);
	glUniform1f(globals::snow_texture_size_location, config::snow_texture_size);

	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, terrain::indicesData.size(), GL_UNSIGNED_INT, 0);
	glUseProgram(0);
	glBindVertexArray(0);

	//SPHERES DRAW
#ifdef DEBUG
	checkGl();
	std::cout << "spheres draw" << std::endl;
#endif
	glUseProgram(shaderProgram::spheres_program);
	glBindVertexArray(spheres::spheresVAO);

	glUniformMatrix4fv(globals::mvp_location, 1, GL_FALSE, glm::value_ptr(model_view_projection));

	glDrawElementsInstanced(GL_TRIANGLES, spheres::indicesData.size(),
	GL_UNSIGNED_INT, 0, spheres::instanceCount);

	glUseProgram(0);
	glBindVertexArray(0);

// DEFFERED SHADING STEP
	if (config::defferedShading) {
#ifdef DEBUG
	checkGl();
	std::cout << "deffered shading step" << std::endl;
#endif
		//LIGHTS DRAW
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, globals::screenResolution.x, globals::screenResolution.y);
		glUseProgram(shaderProgram::deffered_program);
		glBindVertexArray(spheres::spheresVAO);

		if (config::lightVolumes) {
			glDisable(GL_DEPTH_TEST);
			glDepthMask(false);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}

		glUniformMatrix4fv(globals::mvp_location, 1, GL_FALSE, glm::value_ptr(model_view_projection));
		glUniform2iv(globals::screen_size_location, 1, glm::value_ptr(globals::screenResolution));
		glUniform3fv(globals::light_params_location, 1, glm::value_ptr(config::lightParams));
		glUniform3fv(globals::camera_position_location, 1, glm::value_ptr(lastCameraPosition));
		glUniform1ui(globals::ambient_volume_index_location, spheres::instanceCount);

		if (config::lightVolumes)
			glDrawElementsInstanced(GL_TRIANGLES, spheres::indicesData.size(),
			GL_UNSIGNED_INT, 0, spheres::instanceCount + 1);
		else
			glDrawArrays(GL_TRIANGLES, 0, 6);

		if (config::lightVolumes) {
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
		}

		glUseProgram(0);
		glBindVertexArray(0);

		//SKYBOX DRAW

		glUseProgram(shaderProgram::skybox_program);
		glBindVertexArray(skybox::skyboxVAO);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		glUniformMatrix4fv(globals::mvp_location, 1, GL_FALSE, glm::value_ptr(model_view_projection));
		glUniform3fv(globals::camera_position_location, 1, glm::value_ptr(lastCameraPosition));
		glUniform2iv(globals::screen_size_location, 1, glm::value_ptr(globals::screenResolution));

		glDrawElements(GL_TRIANGLES, skybox::indicesData.size(), GL_UNSIGNED_INT, 0);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);

		glUseProgram(0);
		glBindVertexArray(0);
	}

//SWAP BUFFERS
	glfwSwapBuffers(glfwContext::window);
	glfwPollEvents();
	switchConfiguration();
	fps::fps_frame(true);
}

void setup() {
	config::defferedShading = true;
	config::lightVolumes = true;
	shaderProgram::createDefferedProgram();
	shaderProgram::createTerrainProgram();
	shaderProgram::createSpheresProgram();
	shaderProgram::createSnowProgram(); //MUST BE AFTER TERRAIN
	shaderProgram::createSkyboxProgram();
	deffered_render::init();
	terrain::init();
	spheres::init();
	snow::init(); //MUST BE AFTER TERRAIN AND SPHERES
	skybox::init();
	checkGl();
}

}

int main() {
	glfwContext::initGlfw();
	rendering::lastTime = float(glfwGetTime());
	rendering::currentTime = float(glfwGetTime());

	rendering::setup();
	fps::fps_start();
	while (!glfwWindowShouldClose(glfwContext::window)) {
		rendering::render();
	}

	glfwDestroyWindow(glfwContext::window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
