/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <classes/shader.h>
#include <classes/texture.h>

#define CUBE_MAX_BINDINGS 32

namespace CUBE {

class Material
{
protected:
	Texture::Sampler* samplers[CUBE_MAX_BINDINGS];
protected:
	Material();
	virtual void Update(Shader* shader) = 0;
public:
	virtual ~Material();

	void BindTexture(const GLuint binding, Texture& texture,
		GLenum minFilter=GL_LINEAR, GLenum magFilter=GL_LINEAR);

	void Unbind(const GLuint binding);

	friend class ActiveMaterial;
};

class StdMaterial : public Material
{
protected:
	virtual void Update(Shader* shader) override;
public:
	StdMaterial();
	StdMaterial(const vec3& Kd, const vec3& Ks=vec3(), const float Kshininess=0.0f);
public: // attributes
	vec3  ambient;
	vec3  diffuse;
	vec3  specular;
	vec3  emissive;
	float shininess;
};

class ActiveMaterial
{
private:
	Material* material;
	std::list<class ActiveTexture*> textures;
public:
	ActiveMaterial(Material& m, Shader& shader);
	~ActiveMaterial();

	Material* operator->() const
	{
		return material;
	}
	Material& object() const
	{
		return *material;
	}
};

} // CUBE