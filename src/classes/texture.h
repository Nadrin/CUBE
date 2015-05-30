/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <utils/stack.h>
#include <utils/dim.h>

namespace CUBE {

class Texture
{
public:
	#include "sampler.inl"

	enum Channel {
		Diffuse = 0,
		Normal,
		MaxChannels,
	};
protected:
	Dim    size;
	GLenum iformat;
	GLenum format;
	GLenum type;
	int    samples;

	GLuint id;
protected:
	void InitResource(const int components, const ILubyte* pixels);
	bool DetectFormat();

public:
	Texture(const Dim& dim, const GLenum autoformat);
	Texture(const Dim& dim, const int samples, const GLenum autoformat);
	Texture(const Dim& dim, const GLenum iformat, const GLenum format, const GLenum type);
	Texture(const Dim& dim, const int samples, const GLenum iformat, const GLenum format, const GLenum type);
	Texture(const std::string& path, const GLenum overrideType=GL_NONE);
	virtual ~Texture();

	GLenum GetTarget() const;

	GLenum GetType() const    { return type;   }
	GLenum GetFormat() const  { return format; }

	inline GLuint GetID() const { return id; }

	static std::string Prefix;

	friend class Sampler;
};

class ActiveTexture : public ActiveObject<Texture>
{
private:
	GLuint unit;
	Texture::Sampler* samplerObject;

	const bool ownsSampler;
public:
	ActiveTexture(const GLuint u, Texture& t,
		const GLenum minFilter=GL_LINEAR, const GLenum magFilter=GL_LINEAR);
	ActiveTexture(const GLuint u, Texture::Sampler& s);

	~ActiveTexture();

	Texture::Sampler& sampler() const
	{
		return *samplerObject;
	}
};

class ActiveImageBinding : public ActiveObject<const Texture>
{
private:
	GLuint unit;

public:
	ActiveImageBinding(const GLuint u, const Texture& t,
		const GLenum access=GL_READ_ONLY, const GLenum format=GL_RGBA32F);
	~ActiveImageBinding();
};

} // CUBE