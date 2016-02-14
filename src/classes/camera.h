/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <utils/dim.h>
#include <utils/stack.h>
#include <classes/actor.h>

namespace CUBE {

class CameraActor : public Actor
{
public:
	CameraActor();
	CameraActor(const vec3& position, const quat& rotation=quat());

	virtual void Draw() override {}

	mat4& SetPerspective(float fov, float zNear=1.0f, float zFar=1000.0f);
	mat4& SetOrtho(float left, float right, float bottom, float top, float zNear=-1.0f, float zFar=1.0f);
	mat4& SetOrtho(const Dim& size, float zNear=-1.0f, float zFar=1.0f);
	mat4  CalcTransform() const;

	bool IsActive() const;
	static CameraActor* Current();

public: // properties
	mat4 projection;
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