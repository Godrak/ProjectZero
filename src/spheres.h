#ifndef SPHERES_H_
#define SPHERES_H_

#include <vector>
#include <glm/glm.hpp>
#include <GL/gl.h>

namespace speheres {

struct vertex {
	glm::vec3 position;
	glm::vec3 normal;

	vertex(glm::vec3 pos, glm::vec3 norm) :
			position(pos), normal(norm) {
	}
};

std::vector<vertex> vertexData;
std::vector<uint> indicesData;

static const unsigned int rings = 10;
static const unsigned int sectors = 10;

void createUnitSphere() {
	float const R = 1. / (float) (rings - 1);
	float const S = 1. / (float) (sectors - 1);

	for (int r = 0; r < rings; r++) {
		for (int s = 0; s < sectors; s++) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			vertexData.push_back(vertex { { x, y, z }, { x, y, z } });
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

}

#endif /* SPHERES_H_ */
