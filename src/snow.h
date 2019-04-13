#ifndef SNOW_H_
#define SNOW_H_

#include <glm/glm.hpp>
#include "shaders.h"
#include "terrain.h"
#include "spheres.h"

namespace snow {

GLuint snowVAO;
GLuint deform_texture_sampler;

//WARNING the snow part relies on both terrain and spehres being already initialized!!!
void init() {
	//the snow is going to be very similar to the terrain,
	// so I am going to reuse its VAO and bound the deformation texture to it,
	// then Ill just create custom tes evaluation shader and create snow program with terrain shaders.
	snowVAO = terrain::terrainVAO;
	glBindVertexArray(snowVAO);

	//binding deform texture to this VAO
	glBindTexture(GL_TEXTURE_2D, spheres::deformationTexture);

//	glGenSamplers(1, &deform_texture_sampler);
//	glBindSampler(GL_TEXTURE1, deform_texture_sampler);
//	glSamplerParameteri(deform_texture_sampler, GL_TEXTURE_MIN_FILTER, minMagFiler);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagFiler);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	glBindVertexArray(0);
}

}

#endif /* SNOW_H_ */
