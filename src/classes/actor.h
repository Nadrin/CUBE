/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <utils/property.h>

namespace CUBE {

class Shader;

class Actor
{
protected:
	mutable mat4 transformMatrix;
	mutable bool isDirty;

	const Actor* _parent;
	bool isAlwaysDirty;
public:
	struct {
		vec3 position;
		vec3 scale;
		quat rotation;
	} t;
public:
	Actor();
	Actor(const vec3& position, const vec3& scale=vec3(1.0f));
	Actor(const vec3& position, const quat& rotation, const vec3& scale=vec3(1.0f));

	virtual void Draw();
	virtual void Draw(Shader& shader) {}

	const vec3& GetPosition() const { return t.position; }
	const quat& GetRotation() const { return t.rotation; }
	const vec3& GetScale() const    { return t.scale;    }

	void SetPosition(const vec3& p)
	{
		t.position = p;
		isDirty    = true;
	}
	void SetRotation(const quat& r)
	{
		t.rotation = r;
		isDirty    = true;
	}
	void SetScale(const vec3& s)
	{
		t.scale = s;
		isDirty = true;
	}

	void MarkDirty()
	{ 
		isDirty = true;
	}
	void MarkAlawysDirty(bool dirtyFlag)
	{
		if(dirtyFlag)
			isAlwaysDirty = true;
		else {
			isAlwaysDirty = false;
			isDirty       = true;
		}
	}
	bool IsDirty() const
	{
		return isAlwaysDirty || isDirty;
	}

	const Actor& GetParent() const
	{
		return *_parent;
	}
	void SetParent(const Actor& other)
	{
		_parent = &other;
		isDirty = true;
	}

public: // properties
	Property<Actor, vec3, &Actor::GetPosition, &Actor::SetPosition> position;
	Property<Actor, quat, &Actor::GetRotation, &Actor::SetRotation> rotation;
	Property<Actor, vec3, &Actor::GetScale, &Actor::SetScale>       scale;
	Property<Actor, Actor, &Actor::GetParent, &Actor::SetParent>    parent;

	const mat4& transform() const;
};

} // CUBE