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

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
		int mods) {
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

static void cursor_position_callback(GLFWwindow *window, double xpos,
		double ypos) {
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

	window = glfwCreateWindow(globals::screenWidth, globals::screenHeight,
			"ProjectZero", NULL, NULL);
	if (!window)
		exit(-1);

	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(vsync);

}

}

int main(void) {
	glfwContext::initGlfw();
	shaderProgram::createTerrainProgram();
	shaderProgram::createSpheresProgram();
	shaderProgram::createSnowProgram(); //MUST BE AFTER TERRAIN
	terrain::init();
	spheres::init();
	snow::init(); //MUST BE AFTER TERRAIN
	checkGl();
	float lastTime = float(glfwGetTime());
	float time = float(glfwGetTime());

	fps::fps_start();
	while (!glfwWindowShouldClose(glfwContext::window)) {
		time = float(glfwGetTime());
		auto delta = time - lastTime;
		lastTime = time;
		glfwGetFramebufferSize(glfwContext::window, &globals::screenWidth,
				&globals::screenHeight);

		glViewport(0, 0, globals::screenWidth, globals::screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		camera::moveCamera(glfwContext::forth_back);
		camera::moveCamera(glfwContext::up_down);
		camera::moveCamera(glfwContext::left_right);

		glm::mat4x4 model_view_projection = glm::mat4x4();
		camera::applyViewTransform(model_view_projection);
		camera::applyProjectionTransform(model_view_projection);

		//DEFORMATION AND PHYSICS COMPUTE SHADER DISPATCH
		glUseProgram(shaderProgram::spheres_compute_program);
		glBindVertexArray(spheres::spheresVAO);

		glUniform2f(globals::terrain_size_location, config::TERRAIN_SIZE_M.x,
				config::TERRAIN_SIZE_M.y);
		glUniform1f(globals::vertical_scaling_location,
				config::vertical_scaling);
		glUniform3fv(globals::camera_position_location, 1,
				glm::value_ptr(camera::position));
		glUniform1i(globals::heightmap_location, 0);
		glUniform3fv(globals::gravity_location, 1,
				glm::value_ptr(config::gravity));
		glUniform1f(globals::time_delta_location, delta);
		glUniform1f(globals::pixel_resolution_location,
				config::pixel_resolution);
		glUniform1i(globals::deformation_texture_location, 1); //1 is the texture unit with deform texture

		glDispatchCompute(spheres::instanceCount, 1, 1);

		glUseProgram(0);
		glBindVertexArray(0);

		//TERRAIN DRAW
		glUseProgram(shaderProgram::terrain_program);
		glBindVertexArray(terrain::terrainVAO);

		glUniformMatrix4fv(globals::mvp_location, 1, GL_FALSE,
				glm::value_ptr(model_view_projection));

		glUniform2f(globals::terrain_size_location, config::TERRAIN_SIZE_M.x,
				config::TERRAIN_SIZE_M.y);
		glUniform1f(globals::vertical_scaling_location,
				config::vertical_scaling);
		glUniform3fv(globals::camera_position_location, 1,
				glm::value_ptr(camera::position));
		glUniform1i(globals::heightmap_location, 0); //0 is the texture unit containing heightmap

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_PATCHES, terrain::indicesData.size(), GL_UNSIGNED_INT,
				0);
		glUseProgram(0);
		glBindVertexArray(0);

		//SNOW DRAW
		glUseProgram(shaderProgram::snow_program);
		glBindVertexArray(terrain::terrainVAO); //reusing terrain VAO on purpose

		glUniformMatrix4fv(globals::mvp_location, 1, GL_FALSE,
				glm::value_ptr(model_view_projection));

		glUniform2f(globals::terrain_size_location, config::TERRAIN_SIZE_M.x,
				config::TERRAIN_SIZE_M.y);
		glUniform1f(globals::vertical_scaling_location,
				config::vertical_scaling);
		glUniform3fv(globals::camera_position_location, 1,
				glm::value_ptr(camera::position));
		glUniform1i(globals::heightmap_location, 0); //0 is the texture unit containing heightmap
		glUniform1i(globals::deformation_texture_location, 1); //1 is the texture unit containing deformation texture
		glUniform1f(globals::snow_height_location, config::snow_height); //1 is the texture unit containing deformation texture

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_PATCHES, terrain::indicesData.size(), GL_UNSIGNED_INT,
				0);
		glUseProgram(0);
		glBindVertexArray(0);

		//SPHERES DRAW
		glUseProgram(shaderProgram::spheres_program);
		glBindVertexArray(spheres::spheresVAO);

		glUniformMatrix4fv(globals::mvp_location, 1, GL_FALSE,
				glm::value_ptr(model_view_projection));

		glDrawElementsInstanced(GL_TRIANGLES, spheres::indicesData.size(),
		GL_UNSIGNED_INT, 0, spheres::instanceCount);

		glUseProgram(0);
		glBindVertexArray(0);

		//SWAP BUFFERS
		glfwSwapBuffers(glfwContext::window);
		glfwPollEvents();
		if (config::geometryMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		fps::fps_frame(true);
	}

	glfwDestroyWindow(glfwContext::window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
