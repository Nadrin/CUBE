/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/material.h>
#include <classes/shader.h>

using namespace CUBE;

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
}
