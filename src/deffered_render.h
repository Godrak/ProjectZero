#ifndef DEFFERED_RENDER_H_
#define DEFFERED_RENDER_H_

#include <vector>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include "globals.h"
#include "shaders.h"

namespace deffered_render {

GLuint gBuffer;
GLuint gPosition, gNormal, gDiffuse, gDepth;

void init() {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, globals::screenWidth,
			globals::screenHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glNamedFramebufferTexture(gBuffer, GL_COLOR_ATTACHMENT0, gPosition, 0);

	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, globals::screenWidth,
			globals::screenHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glNamedFramebufferTexture(gBuffer, GL_COLOR_ATTACHMENT1, gNormal, 0);

	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &gDiffuse);
	glBindTexture(GL_TEXTURE_2D, gDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, globals::screenWidth,
			globals::screenHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glNamedFramebufferTexture(gBuffer, GL_COLOR_ATTACHMENT2, gDiffuse, 0);

	glActiveTexture(GL_TEXTURE5);
	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, globals::screenWidth,
			globals::screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glNamedFramebufferTexture(gBuffer, GL_DEPTH_ATTACHMENT, gDepth, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2 };
	glNamedFramebufferDrawBuffers(gBuffer, 3, attachments);

	checkGl();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

}

#endif /* DEFFERED_RENDER_H_ */
