/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <utils/stack.h>
#include <utils/notify.h>
#include <utils/parameter.h>
#include <utils/dim.h>

namespace CUBE {

class Texture;
class FrameBuffer;

class Shader
{
public:
	#include "uniform.inl"
protected:
	GLuint vs, fs, gs, cs;
	GLuint program;

	GLuint globalTime;
	GLuint cameraMatrix;
	GLuint modelMatrix;
	GLuint normalMatrix;

	std::string path;
	std::string name;

	mutable Uniform nullUniform;

	std::list<class ShaderParameter*> uniformParameters;
	mutable std::map<std::string, Uniform> uniformCache;

	struct NotifyHandler : public FileNotify::Handler {
		Shader* shader;
		NotifyHandler(Shader* s) : shader(s) {}
		virtual void operator()(const std::string& filename) override;
	} notifyHandler;
protected:
	virtual std::string GetShaderFilename(GLenum type) const;
	std::string GetInfoLog(const GLuint id, GLenum type) const;

	Shader();

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

	bool SetGlobalTime(float time) const;
	bool SetCameraMatrix(const mat4& matrix) const;
	bool SetCameraMatrix(const mat4& projection, const mat4& view) const;
	bool SetModelMatrix(const mat4& matrix) const;

	bool IsActive() const;
	
	Uniform* GetUniform(const std::string& name) const;
	Uniform& operator[](const std::string& name) const;

	static std::string Prefix;
	static Shader* Current();

	friend Shader::NotifyHandler;
	friend class ActiveShader;
};

class ImageShader : public Shader
{
protected:
	virtual std::string GetShaderFilename(GLenum type) const override;
public:
	ImageShader(const std::string& path);
	ImageShader(const ImageShader& other) = delete;
	virtual ~ImageShader();

	void Draw(const BlendFunc& blendFunc=BlendFunc());
	void Draw(Texture& input, const BlendFunc& blendFunc=BlendFunc());
	void Draw(Texture& input, FrameBuffer& output, const BlendFunc& blendFunc=BlendFunc());

	static std::string VertexShaderName;
};

class ComputeShader : public Shader
{
protected:
	virtual std::string GetShaderFilename(GLenum type) const override;
public:
	ComputeShader(const std::string& path);
	ComputeShader(const ComputeShader& other) = delete;
	virtual ~ComputeShader();

	void Dispatch(const Dim& groups) const;
	void DispatchSync(const Dim& groups, const GLenum barrier) const;
};

class ActiveShader : public ActiveObject<Shader>
{
public:
	ActiveShader(Shader& s);
	~ActiveShader();

	Shader::Uniform& operator[](const std::string& name) const
	{
		return objectPtr->operator[](name);
	}

	CUBE_DECLSTACK(ActiveShader);
};

} // CUBE