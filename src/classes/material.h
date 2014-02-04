/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <utils/stack.h>
#include <classes/shader.h>
#include <classes/texture.h>

#define CUBE_MAX_BINDINGS 32

struct aiMaterial;

namespace CUBE {

class Material
{
protected:
	Texture::Sampler* samplers[CUBE_MAX_BINDINGS];
protected:
	virtual void Update(Shader* shader);
public:
	Material(const float Kopacity=1.0f);
	virtual ~Material();

	void BindTexture(const GLuint binding, Texture& texture, GLenum minFilter=GL_LINEAR, GLenum magFilter=GL_LINEAR);
	void Unbind(const GLuint binding);

	bool IsActive() const;
	static Material* Current();

	friend class ActiveMaterial;
public: // attributes
	float opacity;
};

class StdMaterial : public Material
{
protected:
	virtual void Update(Shader* shader) override;
public:
	StdMaterial();
	StdMaterial(const aiMaterial* material);
	StdMaterial(const vec3& Kd, const vec3& Ks=vec3(), const float Kopacity=1.0f);

public: // attributes
	vec3  ambient;
	vec3  diffuse;
	vec3  specular;
	vec3  emissive;
	float shininess;
};

class ActiveMaterial : public ActiveObject<Material>
{
private:
	std::list<class ActiveTexture*> textures;
	bool isBlendingRequired;

	void Init(Shader* shader);
public:
	ActiveMaterial(Material& m);
	ActiveMaterial(Material& m, Shader& shader);
	~ActiveMaterial();

	CUBE_DECLSTACK(ActiveMaterial);
};

} // CUBE