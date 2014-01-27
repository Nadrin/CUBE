/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>

#include <classes/actor.h>
#include <classes/shader.h>
#include <classes/mesh.h>

using namespace CUBE;

Actor::Actor() : parent(nullptr), isDirty(false), isAlwaysDirty(false)
{}

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
		transformMatrix = mat4();
		glm::translate(transformMatrix, t.position);
		transformMatrix *= glm::mat4_cast(t.rotation);
		glm::scale(transformMatrix, t.scale);

		if(parent)
			transformMatrix = parent->transform() * transformMatrix;

		isDirty = false;
	}
	return transformMatrix;
}

void MeshActor::Draw(Shader& shader)
{
	shader.SetModelMatrix(transform());
	
	gltry(glBindVertexArray(mesh->vao));
	gltry(glDrawArrays(GL_TRIANGLES, 0, mesh->numVertices));
	gltry(glBindVertexArray(0));
}
