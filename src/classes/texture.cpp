/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/texture.h>

using namespace CUBE;

std::string Texture::Prefix("textures\\");

Texture::Texture(const Dim& dim, const int comp, bool hdr)
	: size(dim), components(comp), isHDR(hdr)
{
	InitResource(nullptr);
}

Texture::Texture(const std::string& path, bool forceHdr)
{

	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);

	const std::string fp(Prefix+path);
	Core::System::Instance()->Log("Loading texture file: %s\n", fp.c_str());
	if(!ilLoadImage(fp.c_str())) {
		ilDeleteImages(1, &image);
		throw std::runtime_error("Failed to load texture image file: " + fp);
	}

	components  = ilGetInteger(IL_IMAGE_CHANNELS);
	size.Width  = ilGetInteger(IL_IMAGE_WIDTH);
	size.Height = ilGetInteger(IL_IMAGE_HEIGHT);
	size.Depth  = ilGetInteger(IL_IMAGE_DEPTH);

	{
		ILenum format = ilGetInteger(IL_IMAGE_FORMAT);
		ILenum type   = ilGetInteger(IL_IMAGE_TYPE);

		bool convNeeded = false;

		if(forceHdr) {
			if(type != IL_FLOAT) {
				convNeeded = true;
				type       = IL_FLOAT;
			}
		}
		else if(type != IL_FLOAT && type != IL_UNSIGNED_BYTE) {
			convNeeded = true;
			type       = IL_UNSIGNED_BYTE;
		}
		isHDR = (type == IL_FLOAT);

		if(format != IL_RGB && format != IL_RGBA && format != IL_LUMINANCE) {
			convNeeded = true;
			switch(components) {
			case 1:  format = IL_LUMINANCE; break;
			case 4:  format = IL_RGBA; break;
			default: 
				format = IL_RGB;
				components = 3;
			}
		}

		if(convNeeded) {
			if(!ilConvertImage(format, type)) {
				ilDeleteImages(1, &image);
				throw std::runtime_error("Failed to convert texture image into suitable format: " + fp);
			}
		}
	}

	InitResource(ilGetData());
	ilDeleteImages(1, &image);
}

Texture::~Texture()
{
	if(id != 0)
		gltry(glDeleteTextures(1, &id));
}

GLenum Texture::GetTarget() const
{
	if(size.Height == 1 && size.Depth == 1)
		return GL_TEXTURE_1D;
	else if(size.Depth == 1)
		return GL_TEXTURE_2D;
	else
		return GL_TEXTURE_3D;
}

GLenum Texture::GetType() const
{
	if(isHDR)
		return GL_FLOAT;
	else
		return GL_UNSIGNED_BYTE;
}

GLenum Texture::GetFormat() const
{
	switch(components) {
	case 1:
		return GL_R;
	case 2:
		return GL_RG;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	}

	assert(0);
	return 0;
}

void Texture::InitResource(const ILubyte* pixels)
{
	assert(components == 1 
		|| components == 3 
		|| components == 4);

	gltry(glGenTextures(1, &id));

	GLenum target = GetTarget();
	GLenum format = GetFormat();

	gltry(glActiveTexture(GL_TEXTURE0));
	gltry(glBindTexture(target, id));

	switch(target) {
	case GL_TEXTURE_1D:
		gltry(glTexImage1D(target, 0, format, size.Width, 0, format, GetType(), pixels));
		break;
	case GL_TEXTURE_2D:
		gltry(glTexImage2D(target, 0, format, size.Width, size.Height, 0, format, GetType(), pixels));
		break;
	case GL_TEXTURE_3D:
		gltry(glTexImage3D(target, 0, format, size.Width, size.Height, size.Depth, 0, format, GetType(), pixels));
		break;
	default:
		assert(0);
	}

	gltry(glBindTexture(target, 0));
}

ActiveTexture::ActiveTexture(const GLuint u, Texture& t, const GLenum minFilter, const GLenum magFilter) 
	: unit(u), ownsSampler(true)
{
	sampler = new Texture::Sampler(t, minFilter, magFilter);
	sampler->Bind(unit);
}

ActiveTexture::ActiveTexture(const GLuint u, Texture::Sampler& s)
	: unit(u), sampler(&s), ownsSampler(false)
{
	sampler->Bind(unit);
}

ActiveTexture::~ActiveTexture()
{
	sampler->Unbind(unit);
	if(ownsSampler)
		delete sampler;
}
