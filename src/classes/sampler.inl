/* CUBE demo toolkit by MasterM/Asenses */

class Sampler
{
private:
	GLuint   id;
	GLenum   target;
	Texture* texture;
public:
	Sampler(Texture& t, const GLenum minFilter=GL_LINEAR, const GLenum magFilter=GL_LINEAR)
		: texture(&t)
	{
		target = texture->GetTarget();

		gltry(glGenSamplers(1, &id));
		gltry(glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter));
		gltry(glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, magFilter));
	}

	~Sampler()
	{
		gltry(glDeleteSamplers(1, &id));
	}

	void Bind(const GLuint unit) const
	{
		gltry(glActiveTexture(GL_TEXTURE0+unit));
		gltry(glBindTexture(target, texture->id));
		gltry(glBindSampler(unit, id));
	}

	void Unbind(const GLuint unit) const
	{
		gltry(glActiveTexture(GL_TEXTURE0+unit));
		gltry(glBindTexture(target, 0));
		gltry(glBindSampler(unit, 0));
	}

	Texture* GetTexture() const
	{
		return texture;
	}
};
