#ifndef SHADERS_H_
#define SHADERS_H_

#include <epoxy/gl.h>
#include <iostream>

#include "globals.h"

namespace shaderProgram {
GLuint terrain_v_shader, terrain_tc_shader, terrain_tes_shader,
		terrain_f_shader, terrain_program;

bool check_shader(std::string source, GLuint id, GLenum st) {
	GLint logLength;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 2) {
		GLchar *log = new GLchar[logLength];
		glGetShaderInfoLog(id, logLength, &logLength, log);
		log[logLength - 1] = (GLchar) 0;
		std::cout << source << std::endl;
		std::cout << "Shader message: " << log << std::endl;

		delete[] log;
	}

	GLint status;
	glGetShaderiv(id, st, &status);
	return (status == GL_TRUE);
}

bool check_program(GLuint id, GLenum st) {
	GLint logLength;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 2) {
		GLchar *log = new GLchar[logLength];
		glGetProgramInfoLog(id, logLength, &logLength, log);
		log[logLength - 1] = (GLchar) 0;
		std::cout << "Program message: " << log << std::endl;
		delete[] log;
	}

	GLint status;
	glGetProgramiv(id, st, &status);
	return (status == GL_TRUE);
}

void loadAndCompileShader(std::string source, GLuint& destination, GLenum type) {
	std::ifstream shaderSource(source);
	std::string shaderCode;
	getline(shaderSource, shaderCode, (char) shaderSource.eof());
	const char *shaderCodeCString = shaderCode.data();

	destination = glCreateShader(type);
	glShaderSource(destination, 1, &shaderCodeCString, NULL);
	glCompileShader(destination);
	if (!check_shader(source, destination, GL_COMPILE_STATUS))
		exit(-1);
}

void createTerrainProgram() {
	loadAndCompileShader("terrain_shaders/v_shader.glsl", terrain_v_shader,
	GL_VERTEX_SHADER);
	loadAndCompileShader("terrain_shaders/tc_shader.glsl", terrain_tc_shader,
	GL_TESS_CONTROL_SHADER);
	loadAndCompileShader("terrain_shaders/tes_shader.glsl", terrain_tes_shader,
	GL_TESS_EVALUATION_SHADER);
	loadAndCompileShader("terrain_shaders/f_shader.glsl", terrain_f_shader,
	GL_FRAGMENT_SHADER);

	terrain_program = glCreateProgram();
	glAttachShader(terrain_program, terrain_v_shader);
	glAttachShader(terrain_program, terrain_tc_shader);
	glAttachShader(terrain_program, terrain_tes_shader);
	glAttachShader(terrain_program, terrain_f_shader);
	glLinkProgram(terrain_program);
	if (!check_program(terrain_program, GL_LINK_STATUS))
		exit(-1);
}
}

#endif /* SHADERS_H_ */
