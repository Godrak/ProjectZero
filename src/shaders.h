#ifndef SHADERS_H_
#define SHADERS_H_

#include <epoxy/gl.h>
#include <iostream>

#include "globals.h"

namespace shaderProgram {
GLuint vertex_shader, tc_shader, tes_shader, fragment_shader, program;

static bool check_shader(GLuint id, GLenum st) {
	GLint logLength;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 2) {
		GLchar *log = new GLchar[logLength];
		glGetShaderInfoLog(id, logLength, &logLength, log);
		log[logLength - 1] = (GLchar) 0;
		std::cout << "Shader message: " << log << std::endl;

		delete[] log;
	}

	GLint status;
	glGetShaderiv(id, st, &status);
	return (status == GL_TRUE);
}

static bool check_program(GLuint id, GLenum st) {
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

static void createProgram() {
	// shaders:
	std::ifstream vertexShaderSource("VertexShader.glsl");
	std::string vertexShaderCode;
	getline(vertexShaderSource, vertexShaderCode,
			(char) vertexShaderSource.eof());
	const char *vertexShaderCodeCString = vertexShaderCode.data();

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertexShaderCodeCString, NULL);
	glCompileShader(vertex_shader);
	if (!check_shader(vertex_shader, GL_COMPILE_STATUS))
		exit(-1);

	std::ifstream TCShaderSource("TesselationControlShader.glsl");
	std::string TCShaderCode;
	getline(TCShaderSource, TCShaderCode,
			(char) TCShaderSource.eof());
	const char *TCShaderCodeCString = TCShaderCode.data();

	tc_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tc_shader, 1, &TCShaderCodeCString, NULL);
	glCompileShader(tc_shader);
	if (!check_shader(tc_shader, GL_COMPILE_STATUS))
		exit(-1);

	std::ifstream TESShaderSource("TesselationEvaluationShader.glsl");
	std::string TESShaderCode;
	getline(TESShaderSource, TESShaderCode,
			(char) TESShaderSource.eof());
	const char *TESShaderCodeCString = TESShaderCode.data();

	tes_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tes_shader, 1, &TESShaderCodeCString, NULL);
	glCompileShader(tes_shader);
	if (!check_shader(tes_shader, GL_COMPILE_STATUS))
		exit(-1);


	std::ifstream fragmentShaderSource("FragmentShader.glsl");
	std::string fragmentShaderCode;
	getline(fragmentShaderSource, fragmentShaderCode,
			(char) fragmentShaderSource.eof());
	const char *fragmentShaderCodeCString = fragmentShaderCode.data();

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragmentShaderCodeCString, NULL);
	glCompileShader(fragment_shader);
	if (!check_shader(fragment_shader, GL_COMPILE_STATUS))
		exit(-1);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, tc_shader);
	glAttachShader(program, tes_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	if (!check_program(program, GL_LINK_STATUS))
		exit(-1);
}
}





#endif /* SHADERS_H_ */
