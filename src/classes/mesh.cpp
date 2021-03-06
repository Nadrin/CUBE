/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>

#include <classes/mesh.h>
#include <classes/material.h>
#include <classes/texture.h>

#include <assimp/scene.h>

using namespace CUBE;

SubMesh::SubMesh(const aiMesh* mesh)
{
	numVertices = mesh->mNumVertices;
	numFaces    = mesh->mNumFaces;
	materialID  = mesh->mMaterialIndex;

	std::memset(&vbo, 0, sizeof(vbo));

	gltry(glGenVertexArrays(1, &vao));
	gltry(glBindVertexArray(vao));

	vbo[Positions] = CreateVertexBuffer(Positions, 3, mesh->mVertices);
	vbo[Normals]   = CreateVertexBuffer(Normals, 3, mesh->mNormals);
	
	if(mesh->HasTangentsAndBitangents()) {
		vbo[Tangents]   = CreateVertexBuffer(Tangents, 3, mesh->mTangents);
		vbo[Bitangents] = CreateVertexBuffer(Bitangents, 3, mesh->mBitangents);
	}
	if(mesh->HasTextureCoords(0)) {
		vbo[TexCoords0] = CreateVertexBuffer(TexCoords0, mesh->mNumUVComponents[0], mesh->mTextureCoords[0]);
	}

	ibo = CreateIndexBuffer(mesh->mFaces);

	gltry(glBindVertexArray(0));
	gltry(glBindBuffer(GL_ARRAY_BUFFER, 0));
	gltry(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

SubMesh::~SubMesh()
{
	if(vao) gltry(glDeleteVertexArrays(1, &vao));
	if(ibo) gltry(glDeleteBuffers(1, &ibo));

	if(vbo[Positions])  gltry(glDeleteBuffers(1, &vbo[Positions]));
	if(vbo[Normals])    gltry(glDeleteBuffers(1, &vbo[Normals]));
	if(vbo[Tangents])   gltry(glDeleteBuffers(1, &vbo[Tangents]));
	if(vbo[Bitangents]) gltry(glDeleteBuffers(1, &vbo[Bitangents]));
	if(vbo[TexCoords0]) gltry(glDeleteBuffers(1, &vbo[TexCoords0]));
}

GLuint SubMesh::CreateVertexBuffer(int index, int components, const aiVector3D* data)
{
	assert(components <= 3);
	const GLsizei elementSize = components * sizeof(GLfloat);

	GLuint vbo;
	gltry(glGenBuffers(1, &vbo));
	gltry(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	gltry(glBufferData(GL_ARRAY_BUFFER, numVertices * elementSize, nullptr, GL_STATIC_DRAW));

	GLfloat* buffer = (GLfloat*)gltry(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	for(unsigned int i=0; i<numVertices; i++) {
		std::memcpy(&buffer[i*components], &data[i], elementSize);
	}
	gltry(glUnmapBuffer(GL_ARRAY_BUFFER));

	gltry(glVertexAttribPointer(index, components, GL_FLOAT, GL_FALSE, 0, nullptr));
	gltry(glEnableVertexAttribArray(index));
	return vbo;
}

template<typename T>
void SubMesh::GenerateIndices(const aiFace* data)
{
	gltry(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * numFaces * sizeof(T), nullptr, GL_STATIC_DRAW));

	unsigned int numValidFaces = 0;
	T* buffer = (T*)gltry(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	for(unsigned int i=0; i<numFaces; i++) {
		if(data[i].mNumIndices != 3) {
			CUBE_LOG("SubMesh: Warning: Non-triangular face #%02d (%d vertices)\n", i+1, data[i].mNumIndices);
			continue;
		}

		*buffer++ = (T)data[i].mIndices[0];
		*buffer++ = (T)data[i].mIndices[1];
		*buffer++ = (T)data[i].mIndices[2];
		numValidFaces++;
	}
	gltry(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
	numFaces = numValidFaces;
}

GLuint SubMesh::CreateIndexBuffer(const aiFace* data)
{
	GLuint ibo;
	gltry(glGenBuffers(1, &ibo));
	gltry(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

	if(numVertices < std::numeric_limits<unsigned short>::max()) {
		indexType = GL_UNSIGNED_SHORT;
		GenerateIndices<unsigned short>(data);
	}
	else {
		indexType = GL_UNSIGNED_INT;
		GenerateIndices<unsigned int>(data);
	}

	return ibo;
}

Mesh::Mesh(const Flags hints) : hints(hints)
{}

Mesh::Mesh(const std::string& fp, const Flags hints)
	: hints(hints)
{
	InitResource(Assets::Mesh(FromFile, fp, hints));
}

Mesh::Mesh(const Assets::Mesh& data, const Flags hints)
	: hints(hints)
{
	InitResource(data);
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

void Mesh::InitResource(const Assets::Mesh& asset)
{
	const aiScene* scene = asset.GetScene();

	for(unsigned int i=0; i<scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];
		if(asset.IsMeshValid(mesh))
			subMeshes.push_back(new SubMesh(mesh));
	}

	if(!(hints & Hint::WithMaterials))
		return;

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

Shape::Shape(const std::string& desc, const Flags hints) : Mesh(hints)
{
	InitResource(Assets::Mesh(FromString, desc, hints));
}

void MeshActor::DrawDefault(Shader& shader)
{
	for(const auto m : mesh->subMeshes) {
		gltry(glBindVertexArray(m->vao));
		gltry(glDrawElements(GL_TRIANGLES, m->GetFaceCount()*3, m->indexType, 0));
	}
	gltry(glBindVertexArray(0));
}

void MeshActor::DrawWithMaterials(Shader& shader)
{
	for(const auto m : mesh->subMeshes) {
		ActiveMaterial material(*mesh->materials[m->GetMaterialID()], shader);

		gltry(glBindVertexArray(m->vao));
		gltry(glDrawElements(GL_TRIANGLES, m->GetFaceCount()*3, m->indexType, 0));
	}
	gltry(glBindVertexArray(0));
}

void MeshActor::Draw(Shader& shader)
{
	shader.SetModelMatrix(transform());

	if(mesh->hints & Hint::WithMaterials)
		DrawWithMaterials(shader);
	else
		DrawDefault(shader);
}
