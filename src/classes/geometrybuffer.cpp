/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <classes/geometrybuffer.h>
#include <classes/shader.h>
#include <assimp/scene.h>

using namespace CUBE;

GeometryBuffer::GeometryBuffer(unsigned int reserve)
{
	InitResource(reserve);
}

GeometryBuffer::GeometryBuffer(const std::string& path, Flags hints)
{
	Assets::Mesh data(FromFile, path, hints);
	InitResource(data.GetFaceCount());
	LoadMesh(data);
}

GeometryBuffer::GeometryBuffer(const Assets::Mesh& data)
{
	InitResource(data.GetFaceCount());
	LoadMesh(data);
}

void GeometryBuffer::LoadMesh(const Assets::Mesh& data)
{
	assert(data.GetFaceCount() <= numFaces);

	gltry(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	Face* buffer = (Face*)gltry(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	assert(buffer != nullptr);

	const aiScene* scene = data.GetScene();
	for(unsigned int i=0; i<scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];
		if(!data.IsMeshValid(mesh))
			continue;

		bool hasUV = mesh->HasTextureCoords(0);
		for(unsigned int j=0; j<mesh->mNumFaces; j++) {
			const aiFace& face = mesh->mFaces[j];
			if(face.mNumIndices != 3) {
				CUBE_LOG("GeometryBuffer: Non-triangular face #%02d (%d vertices)\n", i+1, face.mNumIndices);
				continue;
			}

			for(int k=0; k<3; k++) {
				Vertex& vtx = buffer->v[k];
				std::memcpy(&vtx.p, &mesh->mVertices[face.mIndices[k]], 3 * sizeof(GLfloat));
				std::memcpy(&vtx.N, &mesh->mNormals[face.mIndices[k]], 3 * sizeof(GLfloat));
				vtx.p.w = 1.0f;

				if(hasUV)
					std::memcpy(&vtx.uv, &mesh->mTextureCoords[0][face.mIndices[k]], 2 * sizeof(GLfloat));

				vtx.T = glm::cross(vtx.N, UpVector);
				if(glm::dot(vtx.T, vtx.T) < Epsilon)
					vtx.T = glm::cross(vtx.N, ForwardVector);
				vtx.S = glm::cross(vtx.N, vtx.T);
			}
			buffer++;
		}
	}

	gltry(glUnmapBuffer(GL_ARRAY_BUFFER));
	gltry(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

GeometryBuffer::~GeometryBuffer()
{
	if(vao) gltry(glDeleteVertexArrays(1, &vao));
	if(vbo) gltry(glDeleteBuffers(1, &vbo));
}

void GeometryBuffer::InitResource(unsigned int numFaces)
{
	this->numFaces = numFaces;

	gltry(glGenBuffers(1, &vbo));
	gltry(glGenVertexArrays(1, &vao));

	gltry(glBindVertexArray(vao));
	gltry(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	gltry(glBufferData(GL_ARRAY_BUFFER, numFaces * sizeof(Face), nullptr, GL_STATIC_DRAW));

	gltry(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
	gltry(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const GLvoid*>(4*sizeof(GLfloat))));
	gltry(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const GLvoid*>(8*sizeof(GLfloat))));

	for(int i=0; i<3; i++)
		gltry(glEnableVertexAttribArray(i));

	gltry(glBindVertexArray(0));
	gltry(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void GeometryActor::Draw(Shader& shader)
{
	shader.SetModelMatrix(transform());

	gltry(glBindVertexArray(buffer->vao));
	gltry(glDrawArrays(GL_TRIANGLES, 0, buffer->GetFaceCount()*3));
	gltry(glBindVertexArray(0));
}

ActiveGeometryBinding::ActiveGeometryBinding(const GLuint u, const GeometryBuffer& buffer)
	: ActiveObject(buffer), unit(u)
{
	gltry(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, unit, buffer.vbo));
}

ActiveGeometryBinding::~ActiveGeometryBinding()
{
	gltry(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, unit, 0));
}
