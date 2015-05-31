/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/assets.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace CUBE;
using namespace CUBE::Assets;

std::string Mesh::Prefix("meshes\\");

Mesh::Mesh(LoadFromFile, const std::string& fp, Flags hints)
	: path(Prefix+fp), importer(new Assimp::Importer)
{
	CUBE_LOG("Loading mesh file: %s\n", path.c_str());
	scene = importer->ReadFile(path.c_str(), GetImportFlags(hints));
	if(!scene) {
		throw std::runtime_error(importer->GetErrorString());
	}
	if(GetFaceCount() == 0) {
		throw std::runtime_error("No suitable mesh data found in: " + GetPath());
	}
}

Mesh::Mesh(LoadFromString, const std::string& data, Flags hints)
	: path("(null)"), importer(new Assimp::Importer)
{
	scene = importer->ReadFileFromMemory(data.c_str(), data.length(), GetImportFlags(hints), "nff");
	if(!scene) {
		throw std::runtime_error(importer->GetErrorString());
	}
	if(GetFaceCount() == 0) {
		throw std::runtime_error("No suitable mesh data found in: " + GetPath());
	}
}

Mesh::~Mesh()
{
	delete importer;
}

Flags Mesh::GetImportFlags(const Flags hints)
{
	Flags flags = 
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_PreTransformVertices |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FixInfacingNormals |
		aiProcess_SortByPType |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords;

	if(hints & Hint::FlatNormals)
		flags |= aiProcess_GenNormals;
	else
		flags |= aiProcess_GenSmoothNormals;

#ifdef _DEBUG
	flags |=
		aiProcess_ValidateDataStructure |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData;
#endif
	return flags;
}


bool Mesh::IsMeshValid(const aiMesh* mesh)
{
	return mesh->HasPositions() && mesh->HasFaces() && mesh->HasNormals();
}

unsigned int Mesh::GetVertexCount() const
{
	unsigned int numVertices = 0;
	for(unsigned int i=0; i<scene->mNumMeshes; i++) {
		if(IsMeshValid(scene->mMeshes[i]))
			numVertices += scene->mMeshes[i]->mNumVertices;
	}
	return numVertices;
}

unsigned int Mesh::GetFaceCount() const
{
	unsigned int numFaces = 0;
	for(unsigned int i=0; i<scene->mNumMeshes; i++) {
		if(IsMeshValid(scene->mMeshes[i]))
			numFaces += scene->mMeshes[i]->mNumFaces;
	}
	return numFaces;
}
