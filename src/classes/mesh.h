/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <classes/texture.h>
#include <utils/cache.h>

struct aiMesh;
struct aiScene;
struct aiMaterial;

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
	GLuint vao;

	unsigned int numVertices;
	unsigned int numFaces;
	unsigned int materialID;

	SubMesh(const aiMesh* mesh);
	virtual ~SubMesh();
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
	const std::string path;
	const bool isWithMaterials;

	std::vector<SubMesh*>     subMeshes;
	std::vector<StdMaterial*> materials;

	std::list<std::shared_ptr<Texture>> textureRef;
	Cache<std::string, Texture>         textureCache;
protected:
	Mesh(Hint hint=Hint::Defaults);

	void InitResource(const aiScene* scene);
	void InitTexture(StdMaterial* material, const aiMaterial* source, Texture::Channel channel);

	virtual unsigned int GetImportFlags() const;
public:
	Mesh(const std::string& fp, Hint=Hint::Defaults);
	Mesh(const Mesh& other) = delete;
	virtual ~Mesh();

	unsigned int GetVertexCount() const;
	unsigned int GetFaceCount() const;

	unsigned int GetSubMeshCount() const { return subMeshes.size(); }
	const std::string& GetPath() const   { return path; }

	static std::string Prefix;

	friend class MeshActor;
};

class Shape : public Mesh
{
public:
	Shape(const std::string& desc);
	Shape(const Shape& other) = delete;
	virtual ~Shape() {}
};

struct CubeShape : public Shape
{
	CubeShape(float size) : Shape("hex 0 0 0 " + std::to_string(size)) {}
};

struct SphereShape : public Shape
{
	SphereShape(float radius) : Shape("s 0 0 0 " + std::to_string(radius)) {}
};

} // CUBE