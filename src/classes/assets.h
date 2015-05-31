/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <assimp/types.h>

struct aiMesh;
struct aiScene;
struct aiMaterial;
struct aiFace;

namespace Assimp {
	class Importer;
}

namespace CUBE {
namespace Assets {

class Mesh
{
public:
	Mesh(LoadFromFile, const std::string& fp, Flags hints=Hint::Defaults);
	Mesh(LoadFromString, const std::string& data, Flags hints=Hint::Defaults);
	Mesh(const Mesh&) = delete;
	~Mesh();

	static Flags GetImportFlags(const Flags hints);
	static bool  IsMeshValid(const aiMesh* mesh);

	const aiScene* GetScene() const    { return scene; }
	const std::string& GetPath() const { return path;  }

	unsigned int GetVertexCount() const;
	unsigned int GetFaceCount() const;
	
	static std::string Prefix;

private:
	std::string path;
	Assimp::Importer* importer;
	const aiScene* scene;
};

} // Assets
} // CUBE
