/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/camera.h>
#include <classes/shader.h>

using namespace CUBE;

CameraActor::CameraActor() : Actor()
{}

CameraActor::CameraActor(const vec3& position, const quat& rotation) : Actor(position, rotation)
{}

mat4& CameraActor::Perspective(const float fov, const float zNear, const float zFar)
{
	ViewportDim viewport;

	projectionMatrix = glm::perspectiveFov(fov, float(viewport.Width), float(viewport.Height), zNear, zFar);
	return projectionMatrix;
}

mat4& CameraActor::Ortho(const Dim& size, const float zNear, const float zFar)
{
	projectionMatrix = glm::ortho(-0.5f*size.Width, 0.5f*size.Width, -0.5f*size.Height, 0.5f*size.Height, zNear, zFar);
	return projectionMatrix;
}

mat4 CameraActor::CalcTransform() const
{
	return projectionMatrix * glm::inverse(transform());
}

bool CameraActor::IsActive() const
{
	return ActiveCamera::Stack.Current() && ActiveCamera::Stack.Current()->InstanceOf(this);
}

CameraActor* CameraActor::Current()
{
	if(ActiveCamera::Stack.Current())
		return ActiveCamera::Stack.Current()->ptr();
	return nullptr;
}

CUBE_STACK(ActiveCamera);

ActiveCamera::ActiveCamera(CameraActor& s) : ActiveObject(s)
{
	CUBE_PUSH;
	assert(!ActiveShader::Stack.IsEmpty());

	Shader& shader = ActiveShader::Stack.Current()->object();
	shader.SetCameraMatrix(objectPtr->CalcTransform());
}

ActiveCamera::ActiveCamera(CameraActor& s, Shader& shader) : ActiveObject(s)
{
	CUBE_PUSH;
	shader.SetCameraMatrix(objectPtr->CalcTransform());
}

ActiveCamera::~ActiveCamera()
{
	CUBE_POP;
}