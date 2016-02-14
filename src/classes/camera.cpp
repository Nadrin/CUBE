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

mat4& CameraActor::SetPerspective(float fov, float zNear, float zFar)
{
	ViewportDim viewport;

	projection = glm::perspective(fov, viewport.AspectRatio(), zNear, zFar);
	return projection;
}

mat4& CUBE::CameraActor::SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	projection = glm::ortho(left, right, bottom, top, zNear, zFar);
	return projection;
}

mat4& CameraActor::SetOrtho(const Dim& size, float zNear, float zFar)
{
	projection = glm::ortho(-0.5f*size.Width, 0.5f*size.Width, -0.5f*size.Height, 0.5f*size.Height, zNear, zFar);
	return projection;
}

mat4 CameraActor::CalcTransform() const
{
	return projection * glm::inverse(transform());
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
	shader.SetCameraPosition(objectPtr->GetPosition());
}

ActiveCamera::ActiveCamera(CameraActor& s, Shader& shader) : ActiveObject(s)
{
	CUBE_PUSH;
	shader.SetCameraMatrix(objectPtr->CalcTransform());
	shader.SetCameraPosition(objectPtr->GetPosition());
}

ActiveCamera::~ActiveCamera()
{
	CUBE_POP;
}