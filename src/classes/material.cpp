/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/material.h>

#include <assimp/scene.h>

using namespace CUBE;

Material::Material(const float Kopacity) : opacity(Kopacity)
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

void Material::Update(Shader* shader)
{
	Shader::Uniform* uniform = shader->GetUniform("Opacity");
	if(uniform)
		(*uniform) = opacity;
}

StdMaterial::StdMaterial() : Material(), diffuse(0.5f), shininess(10.0f)
{}

StdMaterial::StdMaterial(const vec3& Kd, const vec3& Ks, const float Kopacity)
	: Material(Kopacity), diffuse(Kd), specular(Ks), shininess(10.0f)
{}

StdMaterial::StdMaterial(const aiMaterial* material)
	: Material(), shininess(10.0f)
{
	aiColor3D value;
	auto GetValue = [&value]() { return vec3(value.r, value.g, value.b); };
	
	if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, value))
		ambient = GetValue();
	if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, value))
		diffuse = GetValue();
	if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, value))
		specular = GetValue();
	if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, value))
		emissive = GetValue();

	material->Get(AI_MATKEY_SHININESS, shininess);
	material->Get(AI_MATKEY_OPACITY, opacity);
}

void StdMaterial::Update(Shader* shader)
{
	Material::Update(shader);

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

	isBlendingRequired = material->opacity < 1.0f;
	if(isBlendingRequired) {
		gltry(glEnable(GL_BLEND));
	}

	for(GLuint i=0; i<CUBE_MAX_BINDINGS; i++) {
		if(material->samplers[i])
			textures.push_back(new ActiveTexture(i, *material->samplers[i]));
	}
}

ActiveMaterial::~ActiveMaterial()
{
	if(isBlendingRequired) {
		gltry(glDisable(GL_BLEND));
	}
	for(auto activeTexture : textures) {
		delete activeTexture;
	}
}
