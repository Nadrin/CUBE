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

	Uniform& operator[](const std::string& name) const;

	static std::string Prefix;

	friend Shader::NotifyHandler;
	friend class UseShader;
};

class UseShader
{
private:
	Shader* shader;
public:
	UseShader(Shader& s);
	~UseShader();

	Shader::Uniform& operator[](const std::string& name) const
	{
		return shader->operator[](name);
	}
	Shader* operator->() const
	{ 
		return shader;
	}
};

} // CUBE