#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <glm/glm.hpp>
#include "shaders.h"
#include "misc/heightmap.c"

namespace terrain {
GLuint terrainVAO, vertexBuffer, elementBuffer;
GLuint heightmap_tex, heightmap_sampler;
GLint vpos_location = 0;
GLint vcol_location = 1;

struct vertex {
	glm::vec3 position;
	glm::vec3 color;

	vertex(glm::vec3 pos, glm::vec3 col) :
			position(pos), color(col) {
	}
};

std::vector<vertex> vertexData;
std::vector<uint> indicesData;

void prepareData() {
	for (int x = 0; x < config::TERRAIN_X; ++x) {
		for (int z = 0; z < config::TERRAIN_Z; ++z) {
			vertexData.push_back( { { x / config::VERTEX_PER_METER, 0, z / config::VERTEX_PER_METER },
					{ 1.0, 0.0, 0.6 } });
		}
	}
	int offset = 0; //note: if i indices buffer already contains stuff for other objects
	for (int x = 0; x < config::TERRAIN_X - 1; ++x) {
		for (int z = 0; z < config::TERRAIN_Z - 1; ++z) {
			indicesData.push_back(offset + x + 1 + z * config::TERRAIN_X);
			indicesData.push_back(offset + x + z * config::TERRAIN_X);
			indicesData.push_back(offset + x + (z + 1) * config::TERRAIN_X);

			indicesData.push_back(offset + x + 1 + z * config::TERRAIN_X);
			indicesData.push_back(offset + x + (z + 1) * config::TERRAIN_X);
			indicesData.push_back(offset + x + 1 + (z + 1) * config::TERRAIN_X);
		}
	}
}

void initHeightmapTex() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gimp_image.width, gimp_image.height,
			0, GL_RGB, GL_UNSIGNED_BYTE, gimp_image.pixel_data);
}

void init() {
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &heightmap_tex);
	glBindTexture(GL_TEXTURE_2D, heightmap_tex);

	checkGl();

	initHeightmapTex();

	checkGl();

	prepareData();

	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertex),
			vertexData.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesData.size() * sizeof(uint),
			indicesData.data(), GL_STATIC_DRAW);

	// vertex attributes - position:
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
			(void *) 0);
	checkGl();
	// vertex attributes - color:
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
			(void *) (sizeof(glm::vec3)));
	checkGl();

	glBindVertexArray(0);
}

}

#endif /* TERRAIN_H_ */
