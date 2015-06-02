/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/framebuffer.h>

using namespace CUBE;

FrameBuffer::FrameBuffer(const Dim& size, const unsigned int samples)
{
	assert(size.Width && size.Height);

	defaultSize.Width  = size.GetWidth();
	defaultSize.Height = size.GetHeight();
	defaultSamples     = samples ? samples : 1;

	std::memset(&targets, 0, sizeof(targets));
	gltry(glGenFramebuffers(1, &fb));
}

FrameBuffer::~FrameBuffer()
{
	gltry(glDeleteFramebuffers(1, &fb));

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

Dim FrameBuffer::GetEffectiveSize(Dim* size) const
{
	FractDim* fd = dynamic_cast<FractDim*>(size);
	if(fd) {
		fd->FromBaseDim(defaultSize);
	}
	return Dim(size->GetWidth(), size->GetHeight());
}

unsigned int FrameBuffer::GetEffectiveSamples(const unsigned int samples) const
{
	return samples ? samples : defaultSamples;
}

unsigned int FrameBuffer::GetRelevantAttachments(GLenum* buffer) const
{
	unsigned int numAttachments = 0;
	for(unsigned int i=0; i<CUBE_MAX_ATTACHMENTS; i++) {
		if(targets.color[i])
			buffer[numAttachments++] = GL_COLOR_ATTACHMENT0 + i;
	}
	return numAttachments;
}

void FrameBuffer::Attach(Attachment& attachment, const GLenum type, Texture& texture)
{
	assert(!attachment);

	gltry(glBindFramebuffer(GL_FRAMEBUFFER, fb));
	gltry(glFramebufferTexture(GL_FRAMEBUFFER, type, texture.GetID(), 0));
	gltry(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	attachment.texture = &texture;
}

void FrameBuffer::Attach(Attachment& attachment, const GLenum type, const RenderBuffer& rb)
{
	assert(!attachment);

	const Dim size = GetEffectiveSize(rb.Size);
	const unsigned int samples = GetEffectiveSamples(rb.Samples);

	gltry(glGenRenderbuffers(1, &attachment.buffer));
	gltry(glBindRenderbuffer(GL_RENDERBUFFER, attachment.buffer));

	if(samples <= 1) {
		gltry(glRenderbufferStorage(GL_RENDERBUFFER,
			rb.Format, size.GetWidth(), size.GetHeight()));
	}
	else {
		gltry(glRenderbufferStorageMultisample(GL_RENDERBUFFER,
			samples, rb.Format, size.GetWidth(), size.GetHeight()));
	}
	gltry(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	gltry(glBindFramebuffer(GL_FRAMEBUFFER, fb));
	gltry(glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, attachment.buffer));
	gltry(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
	
void FrameBuffer::Detach(Attachment& attachment, const GLenum type)
{
	gltry(glBindFramebuffer(GL_FRAMEBUFFER, fb));
	if(attachment.buffer) {
		gltry(glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, 0));
		gltry(glDeleteRenderbuffers(1, &attachment.buffer));
	}
	else if(attachment.texture) {
		gltry(glFramebufferTexture(GL_FRAMEBUFFER, type, 0, 0));
	}
	gltry(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	std::memset(&attachment, 0, sizeof(Attachment));
}


void FrameBuffer::Validate() const
{
	static const std::map<GLenum, std::string> statusString = {
		{ GL_FRAMEBUFFER_UNDEFINED, "FRAMEBUFFER_UNDEFINED" },
		{ GL_FRAMEBUFFER_UNSUPPORTED, "FRAMEBUFFER_UNSUPPORTED" },
		{ GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, "INCOMPLETE_ATTACHMENT" },
		{ GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, "INCOMPLETE_MISSING_ATTACHMENT" },
		{ GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, "INCOMPLETE_DRAW_BUFFER" },
		{ GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER, "INCOMPLETE_READ_BUFFER" },
		{ GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, "INCOMPLETE_MULTISAMPLE" },
		{ GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS, "INCOMPLETE_LAYER_TARGETS" },
	};

	gltry(glBindFramebuffer(GL_FRAMEBUFFER, fb));
	GLenum status = gltry(glCheckFramebufferStatus(GL_FRAMEBUFFER));
	gltry(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	if(status != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error(std::string("FBO validation failed: ") + statusString.at(status));
	}
}

void FrameBuffer::AttachColor(const unsigned int index, Texture& texture)
{
	assert(index < CUBE_MAX_ATTACHMENTS);
	Attach(targets.color[index], GL_COLOR_ATTACHMENT0+index, texture);
}

void FrameBuffer::AttachColor(const unsigned int index, const RenderBuffer& rb)
{
	assert(index < CUBE_MAX_ATTACHMENTS);
	Attach(targets.color[index], GL_COLOR_ATTACHMENT0+index, rb);
}

void FrameBuffer::DetachColor(const unsigned int index)
{
	assert(index < CUBE_MAX_ATTACHMENTS);
	Detach(targets.color[index], GL_COLOR_ATTACHMENT0+index);
}

void FrameBuffer::AttachDepth(Texture& texture)
{
	Attach(targets.depth, GL_DEPTH_ATTACHMENT, texture);
}

void FrameBuffer::AttachDepth(const RenderBuffer& rb)
{
	Attach(targets.depth, GL_DEPTH_ATTACHMENT, rb);
}

void FrameBuffer::DetachDepth()
{
	Detach(targets.depth, GL_DEPTH_ATTACHMENT);
}

void FrameBuffer::AttachStencil(const RenderBuffer& rb)
{
	Attach(targets.stencil, GL_STENCIL_ATTACHMENT, rb);
}

void FrameBuffer::DetachStencil()
{
	Detach(targets.stencil, GL_STENCIL_ATTACHMENT);
}

bool FrameBuffer::IsActiveDraw() const
{
	return DrawFrameBuffer::Stack.Current() && DrawFrameBuffer::Stack.Current()->InstanceOf(this);
}

bool FrameBuffer::IsActiveRead() const
{
	return ReadFrameBuffer::Stack.Current() && ReadFrameBuffer::Stack.Current()->InstanceOf(this);
}

FrameBuffer* FrameBuffer::CurrentDraw()
{
	if(DrawFrameBuffer::Stack.Current())
		return DrawFrameBuffer::Stack.Current()->ptr();
	return nullptr;
}

FrameBuffer* FrameBuffer::CurrentRead()
{
	if(ReadFrameBuffer::Stack.Current())
		return ReadFrameBuffer::Stack.Current()->ptr();
	return nullptr;
}

CUBE_STACK(DrawFrameBuffer);
CUBE_STACK(ReadFrameBuffer);

DrawFrameBuffer::DrawFrameBuffer(FrameBuffer& fb) : ActiveObject(fb)
{
	CUBE_PUSH;

	GLenum attachments[CUBE_MAX_ATTACHMENTS];
	unsigned int numAttachments = fb.GetRelevantAttachments(attachments);
	assert(numAttachments > 0);

	gltry(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.fb));
	gltry(glDrawBuffers(numAttachments, attachments));
	gltry(glViewport(0, 0, fb.defaultSize.GetWidth(), fb.defaultSize.GetHeight()));
}

DrawFrameBuffer::~DrawFrameBuffer()
{
	ViewportDim viewport;

	gltry(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	gltry(glViewport(0, 0, viewport.GetWidth(), viewport.GetHeight()));
	CUBE_POP;
}

ReadFrameBuffer::ReadFrameBuffer(FrameBuffer& fb) : ActiveObject(fb)
{
	CUBE_PUSH;

	GLenum attachments[CUBE_MAX_ATTACHMENTS];
	assert(fb.GetRelevantAttachments(attachments) > 0);

	gltry(glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.fb));
	gltry(glReadBuffer(attachments[0]));
}

ReadFrameBuffer::~ReadFrameBuffer()
{
	gltry(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	CUBE_POP;
}

void ReadFrameBuffer::Blit(const GLbitfield mask, const GLenum filter) const
{
	const Dim& size       = objectPtr->defaultSize;
	const GLint srcrect[] = { 0, 0, size.GetWidth()-1, size.GetHeight()-1 };

	GLint dstrect[4];
	gltry(glGetIntegerv(GL_VIEWPORT, dstrect));

	gltry(glBlitFramebuffer(
		srcrect[0], srcrect[1], srcrect[2],   srcrect[3],
		dstrect[0], dstrect[1], dstrect[2]-1, dstrect[3]-1,
		mask, filter));
}

void ReadFrameBuffer::Blit(FrameBuffer& other, const GLbitfield mask, const GLenum filter) const
{
	DrawFrameBuffer drawfb(other);
	Blit(mask, filter);
}
