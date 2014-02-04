/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <utils/dim.h>
#include <utils/stack.h>
#include <classes/actor.h>

namespace CUBE {

class CameraActor : public Actor
{
protected:
	mat4 projectionMatrix;
public:
	CameraActor();
	CameraActor(const vec3& position, const quat& rotation=quat());

	virtual void Draw() override {}

	mat4& Perspective(const float fov, const float zNear=1.0f, const float zFar=1000.0f);
	mat4& Ortho(const Dim& size, const float zNear=-1.0f, const float zFar=1.0f);
	mat4  CalcTransform() const;

	bool IsActive() const;
	static CameraActor* Current();
public: // attributes
	vec3& scale() = delete;

	mat4& projection() { return projectionMatrix; }
};

class ActiveCamera : public ActiveObject<CameraActor>
{
public:
	ActiveCamera(CameraActor& c);
	ActiveCamera(CameraActor& c, Shader& shader);
	~ActiveCamera();

	CUBE_DECLSTACK(ActiveCamera);
};

} // CUBE