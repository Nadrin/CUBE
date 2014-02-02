/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

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
	Dim  size;
	int  components;
	bool isHDR;

	GLuint id;
protected:
	void InitResource(const ILubyte* pixels);
public:
	Texture(const Dim& dim, const int comp, bool hdr=false);
	Texture(const std::string& path, bool forceHdr=false);
	virtual ~Texture();

	virtual GLenum GetTarget() const;
	virtual GLenum GetType() const;
	virtual GLenum GetFormat() const;

	static std::string Prefix;

	friend class Sampler;
};

class ActiveTexture
{
private:
	GLuint unit;
	Texture::Sampler* sampler;

	const bool ownsSampler;
public:
	ActiveTexture(const GLuint u, Texture& t,
		const GLenum minFilter=GL_LINEAR, const GLenum magFilter=GL_LINEAR);
	ActiveTexture(const GLuint u, Texture::Sampler& s);

	~ActiveTexture();

	Texture* operator->() const
	{
		return sampler->GetTexture();
	}
	Texture& object() const
	{
		return *sampler->GetTexture();
	}
};

} // CUBE