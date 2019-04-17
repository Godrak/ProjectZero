#ifndef SNOW_H_
#define SNOW_H_

#include <glm/glm.hpp>
#include "shaders.h"
#include "terrain.h"
#include "spheres.h"

namespace snow {

GLuint snowVAO;

//WARNING the snow part relies on both terrain and spheres being already initialized!!!
void init() {
	//the snow is going to be very similar to the terrain,
	// so I am going to reuse its VAO.
	// then Ill just create custom tes evaluation shader and create snow program with terrain shaders.
	snowVAO = terrain::terrainVAO;
}

}

#endif /* SNOW_H_ */
