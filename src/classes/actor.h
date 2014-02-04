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
	vec3& position() { isDirty=true; return t.position; }
	vec3& scale()    { isDirty=true; return t.scale;    }
	quat& rotation() { isDirty=true; return t.rotation; }

	const mat4& transform() const;
};

class Mesh;

class MeshActor : public Actor
{
protected:
	Mesh* mesh;

	void DrawDefault(Shader& shader);
	void DrawWithMaterials(Shader& shader);
public:
	MeshActor(Mesh& mesh)
		: Actor(), mesh(&mesh) {}
	MeshActor(Mesh& mesh, const vec3& position, const vec3& scale=vec3(1.0f))
		: Actor(position, scale), mesh(&mesh) {}
	MeshActor(Mesh& mesh, const vec3& position, const quat& rotation, const vec3& scale=vec3(1.0f))
		: Actor(position, rotation, scale), mesh(&mesh) {}

	Mesh* operator->() const { return mesh;  }
	Mesh& object() const     { return *mesh; }

	using Actor::Draw;
	virtual void Draw(Shader& shader) override;
};

} // CUBE