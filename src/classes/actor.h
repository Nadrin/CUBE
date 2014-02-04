/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Shader;

class Actor
{
protected:
	mutable mat4 transformMatrix;
	mutable bool isDirty;

	const Actor* parent;
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

	Actor& operator<<(const Actor& other)
	{
		parent  = &other;
		isDirty = true;
		return *this;
	}
	const Actor& operator>>(Actor& other) const
	{
		other.parent  = this;
		other.isDirty = true;
		return *this;
	}
public: // attributes
	vec3& position() { MarkDirty(); return t.position; }
	vec3& scale()    { MarkDirty(); return t.scale;    }
	quat& rotation() { MarkDirty(); return t.rotation; }

	const mat4& transform() const;
};

} // CUBE