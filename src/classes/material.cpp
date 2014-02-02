/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/material.h>

using namespace CUBE;

Material::Material()
{
	std::memset(samplers, 0, sizeof(samplers));
}

Material::~Material()
{
	for(GLuint i=0; i<CUBE_MAX_BINDINGS; i++) {
		delete samplers[i];
	}
}

void Material::BindTexture(const GLuint binding, Texture& texture, GLenum minFilter, GLenum magFilter)
{
	assert(binding < CUBE_MAX_BINDINGS);
	samplers[binding] = new Texture::Sampler(texture, minFilter, magFilter);
}

void Material::Unbind(const GLuint binding)
{
	assert(binding < CUBE_MAX_BINDINGS);
	delete samplers[binding];
	samplers[binding] = nullptr;
}

StdMaterial::StdMaterial() : diffuse(0.5f)
{}

StdMaterial::StdMaterial(const vec3& Kd, const vec3& Ks, const float Kshininess)
	: diffuse(Kd), specular(Ks), shininess(Kshininess)
{}

void StdMaterial::Update(Shader* shader)
{
	Shader::Uniform* uniform;

	if(uniform = shader->GetUniform("Ambient"))
		(*uniform) = ambient;
	if(uniform = shader->GetUniform("Diffuse"))
		(*uniform) = diffuse;
	if(uniform = shader->GetUniform("Specular"))
		(*uniform) = specular;
	if(uniform = shader->GetUniform("Emissive"))
		(*uniform) = emissive;
	if(uniform = shader->GetUniform("Shininess"))
		(*uniform) = shininess;
}

ActiveMaterial::ActiveMaterial(Material& m, Shader& shader) : material(&m)
{
	material->Update(&shader);

	for(GLuint i=0; i<CUBE_MAX_BINDINGS; i++) {
		if(material->samplers[i])
			textures.push_back(new ActiveTexture(i, *material->samplers[i]));
	}
}

ActiveMaterial::~ActiveMaterial()
{
	for(auto activeTexture : textures) {
		delete activeTexture;
	}
}
