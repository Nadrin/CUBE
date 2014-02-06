/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/framebuffer.h>

using namespace CUBE;

Framebuffer::Framebuffer()
{
	std::memset(&targets, 0, sizeof(targets));
	gltry(glGenFramebuffers(1, &id));
}

Framebuffer::~Framebuffer()
{
	gltry(glDeleteFramebuffers(1, &id));

	for(unsigned int i=0; i<CUBE_MAX_ATTACHMENTS; i++) {
		if(targets.color[i].buffer) {
			gltry(glDeleteRenderbuffers(1, &targets.color[i].buffer));
		}
	}

	if(targets.depth.buffer) {
		gltry(glDeleteRenderbuffers(1, &targets.depth.buffer));
	}
	if(targets.stencil.buffer) {
		gltry(glDeleteRenderbuffers(1, &targets.stencil.buffer));
	}
}
