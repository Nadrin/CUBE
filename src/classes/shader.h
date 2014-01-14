/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Shader
{
protected:
	GLuint vs, fs, gs;
	GLuint program;
	std::string path;
protected:
	std::string GetShaderFilename(GLenum type) const;

	GLuint CompileShader(GLenum type);
	void   LinkProgram();
	void   DeleteProgram();
public:
	Shader(const std::string& path);
	virtual ~Shader();

	static std::string Prefix;

	friend class UseShader;
};

class UseShader
{
private:
	Shader* shader;
public:
	UseShader(Shader& s);
	~UseShader();

	Shader* operator->() const { return shader; }
};

} // CUBE