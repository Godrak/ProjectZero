#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <glm/glm.hpp>
#include "shaders.h"
#include "misc/heightmap2.c"
#include "misc/rocky_snow/ao.c"
#include "misc/rocky_snow/color.c"
#include "misc/rocky_snow/height.c"
#include "misc/rocky_snow/normal.c"

namespace terrain {
GLuint terrainVAO, vertexBuffer, elementBuffer;
GLuint heightmap_tex, snow_normal_tex, snow_color_tex, snow_ao_tex, snow_height_tex;
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

float readHeightmapValue(glm::vec2 position) {
	float s = position.x / config::terrainSizeM.x;
	float t = position.y / config::terrainSizeM.y;
	auto heightmap_pos_x = s * gimp_image.width;
	auto heightmap_pos_y = t * gimp_image.height;

	int linear_coord = heightmap_pos_x * gimp_image.bytes_per_pixel
			* heightmap_pos_y + heightmap_pos_x * gimp_image.bytes_per_pixel;

	unsigned int value = gimp_image.pixel_data[linear_coord];
	return (value / 255.0f) * config::verticalScaleU;
}

void prepareData() {
	for (int x = 0; x < config::terrainVerticesX; ++x) {
		for (int z = 0; z < config::terrainVerticesZ; ++z) {
			vertexData.push_back(
					{ { x * config::terrainSizeU.x / config::terrainVerticesX,
							0, z * config::terrainSizeU.y
									/ config::terrainVerticesZ },
							{ 0.4, 0.8, 1.0 } });
		}
	}
	int offset = 0; //note: if i indices buffer already contains stuff for other objects
	for (int x = 0; x < config::terrainVerticesX - 1; ++x) {
		for (int z = 0; z < config::terrainVerticesZ - 1; ++z) {
			indicesData.push_back(
					offset + x + 1 + z * config::terrainVerticesX);
			indicesData.push_back(offset + x + z * config::terrainVerticesX);
			indicesData.push_back(
					offset + x + (z + 1) * config::terrainVerticesX);

			indicesData.push_back(
					offset + x + 1 + z * config::terrainVerticesX);
			indicesData.push_back(
					offset + x + (z + 1) * config::terrainVerticesX);
			indicesData.push_back(
					offset + x + 1 + (z + 1) * config::terrainVerticesX);
		}
	}
}

void initHeightmapTex() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gimp_image.width, gimp_image.height,
			0,
			GL_RGB, GL_UNSIGNED_BYTE, gimp_image.pixel_data);
}

void initSnowNormalTex() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snow_normal.width, snow_normal.height,
			0,
			GL_RGB, GL_UNSIGNED_BYTE, snow_normal.pixel_data);
}

void initSnowColorTex() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snow_color.width, snow_color.height,
			0,
			GL_RGB, GL_UNSIGNED_BYTE, snow_color.pixel_data);
}

void initSnowAOTex() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snow_ao.width, snow_ao.height,
			0,
			GL_RGB, GL_UNSIGNED_BYTE, snow_ao.pixel_data);
}

void initSnowHeightTex() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snow_height.width, snow_height.height,
			0,
			GL_RGB, GL_UNSIGNED_BYTE, snow_height.pixel_data);
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
	initHeightmapTex();
	checkGl();

	glActiveTexture(GL_TEXTURE6);
	glGenTextures(1, &snow_normal_tex);
	glBindTexture(GL_TEXTURE_2D, snow_normal_tex);
	initSnowNormalTex();
	checkGl();

	glActiveTexture(GL_TEXTURE8);
	glGenTextures(1, &snow_color_tex);
	glBindTexture(GL_TEXTURE_2D, snow_color_tex);
	initSnowColorTex();
	checkGl();

	glActiveTexture(GL_TEXTURE9);
	glGenTextures(1, &snow_ao_tex);
	glBindTexture(GL_TEXTURE_2D, snow_ao_tex);
	initSnowAOTex();
	checkGl();

	glActiveTexture(GL_TEXTURE10);
	glGenTextures(1, &snow_height_tex);
	glBindTexture(GL_TEXTURE_2D, snow_height_tex);
	initSnowHeightTex();
	checkGl();

	prepareData();
	checkGl();

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
