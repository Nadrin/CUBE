/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <classes/assets.h>
#include <classes/actor.h>
#include <classes/texture.h>
#include <utils/cache.h>

namespace CUBE {

class StdMaterial;

class SubMesh
{
protected:
	enum Binding {
		Positions = 0,
		Normals,
		TexCoords0,
		NumBindings,
	};
protected:
	GLuint vbo[NumBindings];
	GLuint ibo, vao;

	mutable GLenum indexType;

	unsigned int numVertices;
	unsigned int numFaces;
	unsigned int materialID;

	SubMesh(const aiMesh* mesh);
	virtual ~SubMesh();

	GLuint CreateVertexBuffer(int index, int components, const aiVector3D* data);
	GLuint CreateIndexBuffer(const aiFace* data);
private:
	template<typename T>
	void GenerateIndices(const aiFace* data);
public:
	unsigned int GetVertexCount() const { return numVertices; }
	unsigned int GetFaceCount() const   { return numFaces;    }
	unsigned int GetMaterialID() const  { return materialID;  }

	friend class Mesh;
	friend class MeshActor;
};

class Mesh
{
protected:
	const Flags hints;

	std::vector<SubMesh*>     subMeshes;
	std::vector<StdMaterial*> materials;

	std::list<std::shared_ptr<Texture>> textureRef;
	Cache<std::string, Texture>         textureCache;
protected:
	Mesh(const Flags hints=Hint::Defaults);

	void InitResource(const Assets::Mesh& asset);
	void InitTexture(StdMaterial* material, const aiMaterial* source, Texture::Channel channel);

public:
	Mesh(const std::string& fp, const Flags hints=Hint::Defaults);
	Mesh(const Assets::Mesh& data, const Flags hints=Hint::Defaults);
	Mesh(const Mesh& other) = delete;
	virtual ~Mesh();

	unsigned int GetVertexCount() const;
	unsigned int GetFaceCount() const;

	unsigned int GetSubMeshCount() const { return subMeshes.size(); }

	friend class MeshActor;
};

class Shape : public Mesh
{
public:
	Shape(const std::string& desc, const Flags hints=Hint::Defaults);
	Shape(const Shape& other) = delete;
	virtual ~Shape() {}
};

struct CubeShape : public Shape
{
	CubeShape(float size=1.0f) : Shape("hex 0 0 0 " + std::to_string(size), Hint::FlatNormals) {}
};

struct SphereShape : public Shape
{
	SphereShape(float radius=1.0f) : Shape("s 0 0 0 " + std::to_string(radius)) {}
};

struct PlaneShape : public Shape
{
	PlaneShape() : Shape("p 4\n1 0 -1\n1 0 1\n-1 0 1\n-1 0 -1", Hint::FlatNormals) {}
};

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