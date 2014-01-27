/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

struct aiScene;

namespace CUBE {

class Mesh
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

	const std::string path;
protected:
	Mesh();

	void Process(const aiScene* scene);
	virtual unsigned int GetImportFlags() const;
public:
	Mesh(const std::string& fp);
	Mesh(const Mesh& other) = delete;
	virtual ~Mesh();

	const std::string& GetPath() const { return path; }

	unsigned int GetVertexCount() const { return numVertices; }
	unsigned int GetFaceCount() const   { return numFaces;    }

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