/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/texture.h>

using namespace CUBE;

std::string Texture::Prefix("textures\\");

Texture::Texture(const Dim& dim, const GLenum format, const GLenum type)
	: size(dim), samples(1), format(format), type(type)
{
	InitResource(0, nullptr);
}

Texture::Texture(const Dim& dim, const int samples, const GLenum format, const GLenum type)
	: size(dim), samples(samples), format(format), type(type)
{
	assert(samples > 0);
	assert(samples == 1 || (dim.GetHeight() >= 1 && dim.GetDepth() == 1));

	InitResource(0, nullptr);
}

Texture::Texture(const std::string& path, const GLenum overrideType)
	: samples(1)
{
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);

	const std::string fp(Prefix+path);
	CUBE_LOG("Loading texture file: %s\n", fp.c_str());
	if(!ilLoadImage(fp.c_str())) {
		ilDeleteImages(1, &image);
		throw std::runtime_error("Failed to load texture image file: " + fp);
	}

	int components = ilGetInteger(IL_IMAGE_CHANNELS);

	size.Width  = ilGetInteger(IL_IMAGE_WIDTH);
	size.Height = ilGetInteger(IL_IMAGE_HEIGHT);
	size.Depth  = ilGetInteger(IL_IMAGE_DEPTH);

	{
		ILenum format = ilGetInteger(IL_IMAGE_FORMAT);
		ILenum type   = ilGetInteger(IL_IMAGE_TYPE);

		bool convNeeded = false;

		if(overrideType == GL_FLOAT) {
			if(type != IL_FLOAT) {
				convNeeded = true;
				type       = IL_FLOAT;
			}
		}
		else if(type != IL_FLOAT && type != IL_UNSIGNED_BYTE) {
			convNeeded = true;
			type       = IL_UNSIGNED_BYTE;
		}

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

		this->type = (type == IL_FLOAT) ? GL_FLOAT : GL_UNSIGNED_BYTE;
	}

	InitResource(components, ilGetData());
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
		return samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	else
		return GL_TEXTURE_3D;
}

void Texture::InitResource(const int components, const ILubyte* pixels)
{
	assert(components >= 0 && components <= 4);
	static const GLenum formatLUT[] = { GL_INVALID_ENUM, GL_RED, GL_RG, GL_RGB, GL_RGBA };

	GLenum target = GetTarget();

	if(components > 0)
		format = formatLUT[components];
	
	gltry(glGenTextures(1, &id));
	gltry(glActiveTexture(GL_TEXTURE0));
	gltry(glBindTexture(target, id));

	switch(target) {
	case GL_TEXTURE_1D:
		gltry(glTexImage1D(target, 0, format, size.Width, 0, format, GetType(), pixels));
		break;
	case GL_TEXTURE_2D:
		gltry(glTexImage2D(target, 0, format, size.Width, size.Height, 0, format, GetType(), pixels));
		break;
	case GL_TEXTURE_2D_MULTISAMPLE:
		gltry(glTexImage2DMultisample(target, samples, format, size.Width, size.Height, GL_FALSE));
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
	: ActiveObject(t), unit(u), ownsSampler(true)
{
	samplerObject = new Texture::Sampler(t, minFilter, magFilter);
	samplerObject->Bind(unit);
}

ActiveTexture::ActiveTexture(const GLuint u, Texture::Sampler& s)
	: ActiveObject(*s.GetTexture()), unit(u), samplerObject(&s), ownsSampler(false)
{
	samplerObject->Bind(unit);
}

ActiveTexture::~ActiveTexture()
{
	samplerObject->Unbind(unit);
	if(ownsSampler)
		delete samplerObject;
}
