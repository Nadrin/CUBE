/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace CUBE;

std::string Mesh::Prefix("meshes\\");

Mesh::Mesh() : vao(0), numVertices(0), numFaces(0)
{
	std::memset(vbo, 0, sizeof(vbo));
}

Mesh::Mesh(const std::string& fp) : path(Prefix+fp)
{
	Core::System::Instance()->Log("Loading mesh file: %s ...\n", path.c_str());

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), GetImportFlags());
	if(!scene) {
		throw std::runtime_error(importer.GetErrorString());
	}

	Process(scene);
}

Mesh::~Mesh()
{
	if(vao) gltry(glDeleteVertexArrays(1, &vao));

	if(vbo[Positions])  gltry(glDeleteBuffers(1, &vbo[Positions]));
	if(vbo[Normals])    gltry(glDeleteBuffers(1, &vbo[Normals]));
	if(vbo[TexCoords0]) gltry(glDeleteBuffers(1, &vbo[TexCoords0]));
}

void Mesh::Process(const aiScene* scene)
{
	aiMesh* mesh = nullptr;
	for(unsigned int i=0; i<scene->mNumMeshes; i++) {
		if(scene->mMeshes[i]->HasPositions()
			&& scene->mMeshes[i]->HasFaces()
			&& scene->mMeshes[i]->HasNormals())
		{
			mesh = scene->mMeshes[i];
			if(scene->mNumMeshes > 1)
				Core::System::Instance()->Log("More than one sub-mesh present. Using sub-mesh #%d\n", i+1);
		}
	}

	if(!mesh)
		throw std::runtime_error("No suitable mesh data found in: " + path);

	auto CreateBuffer = [&mesh](int index, int components, const aiVector3D* data)
	{
		assert(components <= 3);
		const GLsizei elementSize = components * sizeof(GLfloat);

		GLuint vbo;
		gltry(glGenBuffers(1, &vbo));
		gltry(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		gltry(glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * elementSize, nullptr, GL_STATIC_DRAW));

		GLfloat* buffer = (GLfloat*)gltry(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		for(unsigned int i=0; i<mesh->mNumVertices; i++) {
			std::memcpy(&buffer[i*components], &data[i], elementSize);
		}
		gltry(glUnmapBuffer(GL_ARRAY_BUFFER));

		gltry(glVertexAttribPointer(index, components, GL_FLOAT, GL_FALSE, 0, nullptr));
		gltry(glEnableVertexAttribArray(index));
		return vbo;
	};

	gltry(glGenVertexArrays(1, &vao));
	gltry(glBindVertexArray(vao));

	vbo[Positions] = CreateBuffer(0, 3, mesh->mVertices);
	vbo[Normals]   = CreateBuffer(1, 3, mesh->mNormals);
		
	if(mesh->HasTextureCoords(0))
		vbo[TexCoords0] = CreateBuffer(2, mesh->mNumUVComponents[0], mesh->mTextureCoords[0]);
	else 
		vbo[TexCoords0] = 0;

	gltry(glBindBuffer(GL_ARRAY_BUFFER, 0));
	gltry(glBindVertexArray(0));

	numVertices = mesh->mNumVertices;
	numFaces    = mesh->mNumFaces;
}

unsigned int Mesh::GetImportFlags() const
{
	unsigned int flags = 
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_PreTransformVertices |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FixInfacingNormals |
		aiProcess_SortByPType |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords;

#ifdef _DEBUG
	flags |=
		aiProcess_ValidateDataStructure |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData;
#endif
	return flags;
}

Shape::Shape(const std::string& desc) : Mesh()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(desc.c_str(), desc.length(), GetImportFlags(), "nff");
	if(!scene) {
		throw std::runtime_error(importer.GetErrorString());
	}

	Process(scene);
}