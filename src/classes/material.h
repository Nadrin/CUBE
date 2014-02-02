/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Shader;

class Material
{
protected:
	virtual void Update(Shader* shader) = 0;
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
public:
	ActiveMaterial(Material& m, Shader& shader);

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