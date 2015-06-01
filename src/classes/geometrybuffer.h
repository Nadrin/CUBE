/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <classes/assets.h>
#include <classes/actor.h>

namespace CUBE {

struct Vertex
{
	GLfloat position[4];
	GLfloat normal[3];
	GLfloat _padding0;
	GLfloat uv[2];
	GLfloat _padding1[2];
};
struct Face
{
	Vertex v[3];
};

class GeometryBuffer
{
public:
	GeometryBuffer(unsigned int reserve=0);
	GeometryBuffer(const std::string& path, Flags hints=Hint::Defaults);
	GeometryBuffer(const Assets::Mesh& data);
	~GeometryBuffer();

	void LoadMesh(const Assets::Mesh& data);
	unsigned int GetFaceCount() const { return numFaces; }

protected:
	void InitResource(unsigned int numFaces);

	GLuint vbo, vao;
	unsigned int numFaces;

	friend class GeometryActor;
	friend class ActiveGeometryBinding;
};

class GeometryActor : public Actor
{
protected:
	GeometryBuffer* buffer;

public:
	GeometryActor(GeometryBuffer& buffer)
		: Actor(), buffer(&buffer) {}
	GeometryActor(GeometryBuffer& buffer, const vec3& position, const vec3& scale=vec3(1.0f))
		: Actor(position, scale), buffer(&buffer) {}
	GeometryActor(GeometryBuffer& buffer, const vec3& position, const quat& rotation, const vec3& scale=vec3(1.0f))
		: Actor(position, rotation, scale), buffer(&buffer) {}

	using Actor::Draw;
	virtual void Draw(Shader& shader) override;
};

class ActiveGeometryBinding : public ActiveObject<const GeometryBuffer>
{
private:
	GLuint unit;

public:
	ActiveGeometryBinding(const GLuint u, const GeometryBuffer& buffer);
	~ActiveGeometryBinding();
};

} // CUBE