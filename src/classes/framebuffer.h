/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <classes/texture.h>

namespace CUBE {

struct RenderBuffer
{
private:
	GLenum       Format;
	Dim*         Size;
	unsigned int Samples;

	friend class FrameBuffer;
public:
	RenderBuffer(const GLenum format, const Dim& size=ViewportDim(), const unsigned int samples=0)
		: Format(format), Size(size.Clone()), Samples(samples) {}
	RenderBuffer(const GLenum format, const unsigned int samples)
		: Format(format), Size(new ViewportDim()), Samples(samples) {}

	~RenderBuffer()
	{
		delete Size;
	}
};

class FrameBuffer
{
protected:
	GLuint fb;

	Dim          defaultSize;
	unsigned int defaultSamples;

	struct Attachment {
		const Texture* texture;
		GLuint buffer;

		operator bool() const
		{
			return texture || buffer;
		}
	};
	struct {
		Attachment color[CUBE_MAX_ATTACHMENTS];
		Attachment depth;
		Attachment stencil;
	} targets;
protected:
	Dim          GetEffectiveSize(Dim* size) const;
	unsigned int GetEffectiveSamples(const unsigned int samples) const;
	unsigned int GetRelevantAttachments(GLenum* buffer) const;

	void Attach(Attachment& attachment, const GLenum type, const Texture& texture);
	void Attach(Attachment& attachment, const GLenum type, const RenderBuffer& rb);
	void Detach(Attachment& attachment, const GLenum type);
public:
	FrameBuffer(const Dim& size, const unsigned int samples=0);
	virtual ~FrameBuffer();

	inline GLuint GetID() const { return fb; }

	void AttachColor(const unsigned int index, const Texture& texture);
	void AttachColor(const unsigned int index, const RenderBuffer& rb);
	void DetachColor(const unsigned int index);

	void AttachDepth(const Texture& texture);
	void AttachDepth(const RenderBuffer& rb);
	void DetachDepth();

	void AttachStencil(const RenderBuffer& rb);
	void DetachStencil();

	void Validate() const;

	bool IsActiveRead() const;
	bool IsActiveDraw() const;

	static FrameBuffer* CurrentRead();
	static FrameBuffer* CurrentDraw();

	friend class DrawFrameBuffer;
	friend class ReadFrameBuffer;
};

class DrawFrameBuffer : public ActiveObject<FrameBuffer>
{
public:
	DrawFrameBuffer(FrameBuffer& fb);
	~DrawFrameBuffer();

	CUBE_DECLSTACK(DrawFrameBuffer);
};

class ReadFrameBuffer : public ActiveObject<FrameBuffer>
{
public:
	ReadFrameBuffer(FrameBuffer& fb);
	~ReadFrameBuffer();

	void Blit(const GLbitfield mask, const GLenum filter=GL_NEAREST) const;
	void Blit(FrameBuffer& other, const GLbitfield mask, const GLenum filter=GL_NEAREST) const;

	CUBE_DECLSTACK(ReadFrameBuffer);
};

} // CUBE