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

void Actor::Draw()
{
	assert(!ActiveShader::Stack.IsEmpty());
	Draw(ActiveShader::Stack.Current()->object());
}
