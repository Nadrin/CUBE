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

	GLuint vbo[NumBindings];
	GLuint vao;

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

	static std::string Prefix;
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