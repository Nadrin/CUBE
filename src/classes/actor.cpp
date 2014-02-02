/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>

#include <classes/actor.h>
#include <classes/shader.h>
#include <classes/mesh.h>
#include <classes/material.h>

using namespace CUBE;

Actor::Actor() : parent(nullptr), isDirty(false), isAlwaysDirty(false)
{
	t.scale = vec3(1.0f);
}

Actor::Actor(const vec3& position, const vec3& scale)
	: parent(nullptr), isDirty(true), isAlwaysDirty(false)
{
	t.position = position;
	t.scale    = scale;
}

Actor::Actor(const vec3& position, const quat& rotation, const vec3& scale)
	: parent(nullptr), isDirty(true), isAlwaysDirty(false)
{
	t.position = position;
	t.rotation = rotation;
	t.scale    = scale;
}

const mat4& Actor::transform() const
{
	if(isDirty) {
		transformMatrix  = glm::translate(mat4(), t.position);
		transformMatrix *= glm::mat4_cast(t.rotation);
		transformMatrix  = glm::scale(transformMatrix, t.scale);

		if(parent)
			transformMatrix = parent->transform() * transformMatrix;

		isDirty = false;
	}
	return transformMatrix;
}

void MeshActor::DrawDefault(Shader& shader)
{
	for(const auto m : mesh->subMeshes) {
		gltry(glBindVertexArray(m->vao));
		gltry(glDrawArrays(GL_TRIANGLES, 0, m->GetVertexCount()));
	}
	gltry(glBindVertexArray(0));
}

void MeshActor::DrawWithMaterials(Shader& shader)
{
	for(const auto m : mesh->subMeshes) {
		ActiveMaterial material(*mesh->materials[m->GetMaterialID()], shader);

		gltry(glBindVertexArray(m->vao));
		gltry(glDrawArrays(GL_TRIANGLES, 0, m->GetVertexCount()));
	}
	gltry(glBindVertexArray(0));
}

void MeshActor::Draw(Shader& shader)
{
	shader.SetModelMatrix(transform());

	if(mesh->isWithMaterials)
		DrawWithMaterials(shader);
	else
		DrawDefault(shader);
}
