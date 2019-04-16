#ifndef SNOW_H_
#define SNOW_H_

#include <glm/glm.hpp>
#include "shaders.h"
#include "terrain.h"
#include "spheres.h"

namespace snow {

GLuint snowVAO;
GLuint deform_texture_sampler;

//WARNING the snow part relies on both terrain and spheres being already initialized!!!
void init() {
	//the snow is going to be very similar to the terrain,
	// so I am going to reuse its VAO and bound the deformation texture to it,
	// then Ill just create custom tes evaluation shader and create snow program with terrain shaders.
	snowVAO = terrain::terrainVAO;
	glBindVertexArray(snowVAO);

	//binding deform texture to this VAO
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, spheres::deformationTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindVertexArray(0);
}

}

#endif /* SNOW_H_ */
