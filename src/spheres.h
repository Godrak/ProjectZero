#ifndef SPHERES_H_
#define SPHERES_H_

#include <vector>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include "globals.h"
#include "shaders.h"

namespace spheres {

struct vertex {
	glm::vec3 position;

	vertex(glm::vec3 pos) :
			position(pos) {
	}
};

struct sphereInfo {
	glm::vec3 size;
	glm::vec3 color;
	glm::vec3 worldPosition;
	glm::vec3 speed = { 0, 0, 0 };
	glm::vec3 velocity = { 0, 0, 0 };
	float artistsScale;

	sphereInfo(glm::vec3 s, glm::vec3 col, glm::vec3 pos, float artScale) :
			size(s), color(col), worldPosition(pos), artistsScale(artScale) {

	}
};

std::vector<vertex> vertexData;
std::vector<uint> indicesData;
std::vector<sphereInfo> spheres;

const unsigned int rings = 10;
const unsigned int sectors = 10;
const unsigned int instanceCount = 200;

GLuint spheresVAO, vertexBuffer, elementBuffer, instancingBuffer;
GLuint deformationTexture;
GLint vertexPositionLocation = 0;
GLint instancingBufferBinding = 0;

void createUnitSphere() {
	float const R = 1. / (float) (rings - 1);
	float const S = 1. / (float) (sectors - 1);

	for (int r = 0; r < rings; r++) {
		for (int s = 0; s < sectors; s++) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			vertexData.push_back(vertex { { x, y, z } });
		}
	}

	for (int r = 0; r < rings; r++)
		for (int s = 0; s < sectors; s++) {
			indicesData.push_back(r * sectors + s + 1);
			indicesData.push_back(r * sectors + s);
			indicesData.push_back((r + 1) * sectors + s);

			indicesData.push_back(r * sectors + s + 1);
			indicesData.push_back((r + 1) * sectors + s);
			indicesData.push_back((r + 1) * sectors + s + 1);
		}
}

void prepareData() {
	createUnitSphere();

	for (int i = 0; i < instanceCount; ++i) {
		auto s = config::sphereMinHeightU
				+ (rand() / float(RAND_MAX))
						* (config::sphereMaxHeightU - config::sphereMinHeightU);
		glm::vec3 size = { s, s, s };
		float red = rand() / float(RAND_MAX);
		float green = rand() / float(RAND_MAX);
		float blue = 2 - red - green;

		glm::vec3 color = { red, green, blue };
		glm::vec3 pos = { config::terrainSizeU.x * (rand() / float(RAND_MAX)),
				config::initSpheresAltitudeU, config::terrainSizeU.y
						* (rand() / float(RAND_MAX)) };
		spheres.push_back(sphereInfo { size, color, pos, 1/(size.x*size.x) });
	}
}

void initDeformationTexture() {
	std::vector<uint32_t> init_val(1024 * 1024, 1 << 31);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, 1024, 1024, 0, GL_RED_INTEGER,
	GL_UNSIGNED_INT, init_val.data());
}

void init() {
	glGenVertexArrays(1, &spheresVAO);
	glBindVertexArray(spheresVAO);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

	glGenBuffers(1, &instancingBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instancingBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, instancingBufferBinding,
			instancingBuffer);

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &deformationTexture);
	glBindTexture(GL_TEXTURE_2D, deformationTexture);

	//this one took very long to find out
	glBindImageTexture(1, deformationTexture, 0, GL_FALSE, 0, GL_READ_WRITE,
	GL_R32UI);

	initDeformationTexture();

	checkGl();

	prepareData();

	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertex),
			vertexData.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesData.size() * sizeof(uint),
			indicesData.data(), GL_STATIC_DRAW);
	glBufferData(GL_SHADER_STORAGE_BUFFER, instanceCount * sizeof(sphereInfo),
			spheres.data(), GL_STATIC_DRAW);

// vertex attributes - position:
	glEnableVertexAttribArray(vertexPositionLocation);
	glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE,
			sizeof(vertex), (void *) 0);
	checkGl();

	glBindVertexArray(0);
}

}

#endif /* SPHERES_H_ */
