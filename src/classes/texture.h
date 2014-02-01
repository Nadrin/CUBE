/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <utils/dim.h>

namespace CUBE {

class Texture
{
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

	friend class ActiveTexture;
};

class ActiveTexture
{
private:
	GLenum   target;
	GLuint   unit, sampler;
	Texture* texture;
public:
	ActiveTexture(const GLuint u, Texture& t,
		const GLenum minFilter=GL_LINEAR, const GLenum magFilter=GL_LINEAR);
	~ActiveTexture();

	Texture* operator->() const
	{
		return texture;
	}
	Texture& object() const
	{
		return *texture;
	}
};

} // CUBE