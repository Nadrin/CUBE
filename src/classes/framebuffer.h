/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <classes/texture.h>

namespace CUBE {

class Framebuffer
{
protected:
	GLuint id;

	struct Attachment {
		Texture* texture;
		GLuint   buffer;
	};
	struct {
		Attachment color[CUBE_MAX_ATTACHMENTS];
		Attachment depth;
		Attachment stencil;
	} targets;
public:
	Framebuffer();
	virtual ~Framebuffer();

	void AttachColor(const unsigned int index, const Texture& texture);
	void AttachColor(const unsigned int index);
	void DetachColor(const unsigned int index);

	void AttachDepth(const Texture& texture);
	void DetachDepth();

	void AttachStencil();
	void DetachStencil();
};

} // CUBE