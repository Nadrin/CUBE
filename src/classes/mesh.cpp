/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>

#include <classes/mesh.h>
#include <classes/material.h>
#include <classes/texture.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace CUBE;

std::string Mesh::Prefix("meshes\\");

SubMesh::SubMesh(const aiMesh* mesh)
{
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
	materialID  = mesh->mMaterialIndex;
}

SubMesh::~SubMesh()
{
	if(vao) gltry(glDeleteVertexArrays(1, &vao));

	if(vbo[Positions])  gltry(glDeleteBuffers(1, &vbo[Positions]));
	if(vbo[Normals])    gltry(glDeleteBuffers(1, &vbo[Normals]));
	if(vbo[TexCoords0]) gltry(glDeleteBuffers(1, &vbo[TexCoords0]));
}

Mesh::Mesh(Hint hint) : isWithMaterials(hint == Hint::WithMaterials)
{}

Mesh::Mesh(const std::string& fp, Hint hint)
	: path(Prefix+fp), isWithMaterials(hint == Hint::WithMaterials)
{
	Core::System::Instance()->Log("Loading mesh file: %s ...\n", path.c_str());

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), GetImportFlags());
	if(!scene) {
		throw std::runtime_error(importer.GetErrorString());
	}

	InitResource(scene);
}

Mesh::~Mesh()
{
	for(auto it : materials) {
		delete it;
	}
	for(auto it : subMeshes) {
		delete it;
	}

	textureRef.clear();
	textureCache.Flush();
}

void Mesh::InitResource(const aiScene* scene)
{
	for(unsigned int i=0; i<scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];
		if(mesh->HasPositions() && mesh->HasFaces() && mesh->HasNormals()) {
			subMeshes.push_back(new SubMesh(mesh));
		}
	}

	if(subMeshes.size() == 0)
		throw std::runtime_error("No suitable mesh data found in: " + path);

	for(unsigned int i=0; i<scene->mNumMaterials; i++) {
		const aiMaterial* material  = scene->mMaterials[i];

		StdMaterial* materialObject = new StdMaterial(scene->mMaterials[i]);
		InitTexture(materialObject, material, Texture::Diffuse);
		materials.push_back(materialObject);
	}
}

void Mesh::InitTexture(StdMaterial* material, const aiMaterial* source, Texture::Channel channel)
{
	aiTextureType type;

	switch(channel) {
	case Texture::Diffuse: type = aiTextureType_DIFFUSE; break;
	default: assert(0);
	}

	if(!source->GetTextureCount(type))
		return;

	aiString path;
	std::shared_ptr<Texture> texture;

	source->GetTexture(type, 0, &path);

	texture = textureCache.Get(path.C_Str());
	if(!texture)
		texture = textureCache.Insert(path.C_Str(), new Texture(path.C_Str()));

	material->BindTexture(channel, *texture.get());
	textureRef.push_back(texture);
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

unsigned int Mesh::GetVertexCount() const
{
	unsigned int ret=0;
	for(auto mesh : subMeshes) {
		ret += mesh->GetVertexCount();
	}
	return ret;
}

unsigned int Mesh::GetFaceCount() const
{
	unsigned int ret=0;
	for(auto mesh : subMeshes) {
		ret += mesh->GetFaceCount();
	}
	return ret;
}

Shape::Shape(const std::string& desc) : Mesh()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(desc.c_str(), desc.length(), GetImportFlags(), "nff");
	if(!scene) {
		throw std::runtime_error(importer.GetErrorString());
	}

	InitResource(scene);
}