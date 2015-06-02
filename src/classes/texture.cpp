/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/texture.h>

#include <glm/gtc/random.hpp>

using namespace CUBE;

std::string Texture::Prefix("textures\\");

Texture::Texture(const Dim& dim, const GLenum autoformat)
	: size(dim), samples(1), iformat(autoformat)
{
	assert(DetectFormat());
	InitResource(0, nullptr);
}

Texture::Texture(const Dim& dim, const int samples, const GLenum autoformat)
	: size(dim), samples(samples), iformat(autoformat)
{
	assert(samples > 0);
	assert(samples == 1 || (dim.GetHeight() >= 1 && dim.GetDepth() == 1));

	assert(DetectFormat());
	InitResource(0, nullptr);
}

Texture::Texture(const Dim& dim, const GLenum iformat, const GLenum format, const GLenum type)
	: size(dim), samples(1), iformat(iformat), format(format), type(type)
{
	InitResource(0, nullptr);
}

Texture::Texture(const Dim& dim, const int samples, const GLenum iformat, const GLenum format, const GLenum type)
	: size(dim), samples(samples), iformat(iformat), format(format), type(type)
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

Texture::Texture(TextureGenColorType, const Dim& dim, const vec4& color, const GLenum type)
{
	Generate(dim, [&color](unsigned int, unsigned int) { return color; }, type);
}

Texture::Texture(TextureGenNoiseType, const Dim& dim, unsigned int seed, const GLenum type)
{
	std::srand(seed);
	Generate(dim, [](unsigned int, unsigned int) { return glm::linearRand(vec4(0.0f), vec4(1.0f)); }, type);
}

Texture::Texture(TextureGenFunctionType, const Dim& dim, std::function<vec4(unsigned int, unsigned int)> generator, const GLenum type)
{
	Generate(dim, generator, type);
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

	static const GLenum formatLUT[]   = { GL_INVALID_ENUM, GL_RED, GL_RG, GL_RGB, GL_RGBA };
	static const GLenum iformatLUTi[] = { GL_INVALID_ENUM, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 };
	static const GLenum iformatLUTf[] = { GL_INVALID_ENUM, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F };

	GLenum target = GetTarget();

	if(components > 0) {
		format = formatLUT[components];
		if(type == GL_FLOAT)
			iformat = iformatLUTf[components];
		else
			iformat = iformatLUTi[components];
	}
	
	gltry(glGenTextures(1, &id));
	gltry(glActiveTexture(GL_TEXTURE0));
	gltry(glBindTexture(target, id));

	switch(target) {
	case GL_TEXTURE_1D:
		gltry(glTexImage1D(target, 0, iformat, size.Width, 0, format, type, pixels));
		break;
	case GL_TEXTURE_2D:
		gltry(glTexImage2D(target, 0, iformat, size.Width, size.Height, 0, format, type, pixels));
		break;
	case GL_TEXTURE_2D_MULTISAMPLE:
		gltry(glTexImage2DMultisample(target, samples, iformat, size.Width, size.Height, GL_FALSE));
		break;
	case GL_TEXTURE_3D:
		gltry(glTexImage3D(target, 0, iformat, size.Width, size.Height, size.Depth, 0, format, GetType(), pixels));
		break;
	default:
		assert(0);
	}

	gltry(glBindTexture(target, 0));
}

void Texture::Generate(const Dim& dim, std::function<vec4(unsigned int, unsigned int)> generator, const GLenum type)
{
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);

	const unsigned int numBytes = dim.GetWidth() * dim.GetHeight();
	vec4* pixels = new vec4[numBytes];
	for(unsigned int y=0; y<dim.GetHeight(); y++) {
		for(unsigned int x=0; x<dim.GetWidth(); x++) {
			pixels[y*dim.GetWidth() + x] = generator(x, y);
		}
	}
	ilTexImage(dim.GetWidth(), dim.GetHeight(), 1, 4, IL_RGBA, IL_FLOAT, reinterpret_cast<void*>(pixels));
	delete[] pixels;

	if(type == GL_UNSIGNED_BYTE) {
		this->iformat = GL_RGBA8;
		this->type    = GL_UNSIGNED_BYTE;
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	}
	else {
		this->iformat = GL_RGBA32F;
		this->type    = GL_FLOAT;
	}
	this->format = GL_RGBA;

	size.Width  = ilGetInteger(IL_IMAGE_WIDTH);
	size.Height = ilGetInteger(IL_IMAGE_HEIGHT);
	size.Depth  = ilGetInteger(IL_IMAGE_DEPTH);

	InitResource(4, ilGetData());
	ilDeleteImages(1, &image);
}

bool Texture::DetectFormat()
{
	switch(iformat)
	{
	case GL_R8:
	case GL_R16:
	case GL_R32UI:
		type   = GL_UNSIGNED_BYTE;
		format = GL_RED;
		break;
	case GL_RG8:
	case GL_RG16:
	case GL_RG32UI:
		type   = GL_UNSIGNED_BYTE;
		format = GL_RG;
		break;
	case GL_RGB8:
	case GL_RGB16:
	case GL_RGB32UI:
		type   = GL_UNSIGNED_BYTE;
		format = GL_RGB;
		break;
	case GL_RGBA8:
	case GL_RGBA16:
	case GL_RGBA32UI:
		type   = GL_UNSIGNED_BYTE;
		format = GL_RGBA;
		break;
	case GL_R16F:
	case GL_R32F:
		type   = GL_FLOAT;
		format = GL_RED;
		break;
	case GL_RG16F:
	case GL_RG32F:
		type   = GL_FLOAT;
		format = GL_RG;
		break;
	case GL_RGB16F:
	case GL_RGB32F:
		type   = GL_FLOAT;
		format = GL_RGB;
		break;
	case GL_RGBA16F:
	case GL_RGBA32F:
		type   = GL_FLOAT;
		format = GL_RGBA;
		break;
	default:
		return false;
	}
	return true;
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

ActiveImageBinding::ActiveImageBinding(const GLuint u, const Texture& t, const GLenum access, const GLenum format)
	: ActiveObject(t), unit(u)
{
	gltry(glBindImageTexture(unit, object().GetID(), 0, GL_FALSE, 0, access, format));
}

ActiveImageBinding::~ActiveImageBinding()
{
	gltry(glBindImageTexture(unit, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
}
