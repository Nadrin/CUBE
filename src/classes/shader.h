/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <utils/notify.h>

namespace CUBE {

class Shader
{
public:
	#include "uniform.inl"
protected:
	GLuint vs, fs, gs;
	GLuint program;

	GLuint cameraMatrix;
	GLuint modelMatrix;
	GLuint normalMatrix;

	std::string path;
	std::string name;
	mutable bool isActive;

	std::list<class ShaderParameter*> uniformParameters;
	mutable std::map<std::string, Uniform> uniformCache;

	struct NotifyHandler : public FileNotify::Handler {
		Shader* shader;
		NotifyHandler(Shader* s) : shader(s) {}
		virtual void operator()(const std::string& filename) override;
	} notifyHandler;
protected:
	std::string GetShaderFilename(GLenum type) const;
	std::string GetInfoLog(const GLuint id, GLenum type) const;

	GLuint CompileShader(GLenum type);
	bool   ReloadShader(GLenum type, GLuint& id);
	void   DeleteShader(GLenum type, GLuint& id);
	void   LinkProgram();
	void   DeleteProgram();
	void   ValidateUniforms();
	void   CreateParameters();
public:
	Shader(const std::string& path);
	Shader(const Shader& other) = delete;
	virtual ~Shader();

	const std::string& GetPath() const { return path; }
	const std::string& GetName() const { return name; }

	bool SetCameraMatrix(const mat4& matrix) const;
	bool SetCameraMatrix(const mat4& projection, const mat4& view) const;
	bool SetModelMatrix(const mat4& matrix) const;

	Uniform& operator[](const std::string& name) const;

	static std::string Prefix;

	friend Shader::NotifyHandler;
	friend class ActiveShader;
};

class ActiveShader
{
private:
	Shader* shader;
public:
	ActiveShader(Shader& s);
	~ActiveShader();

	Shader::Uniform& operator[](const std::string& name) const
	{
		return shader->operator[](name);
	}
	Shader* operator->() const
	{ 
		return shader;
	}
	Shader& object() const
	{
		return *shader;
	}
};

} // CUBE