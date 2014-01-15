/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Shader
{
protected:
	GLuint vs, fs, gs;
	GLuint program;
	std::string path;

	struct NotifyHandler : public FileNotify::Handler {
		Shader* shader;
		NotifyHandler(Shader* s) : shader(s) {}
		virtual void operator()(const std::string& filename) override;
	} notifyHandler;
protected:
	std::string GetShaderFilename(GLenum type) const;

	GLuint CompileShader(GLenum type);
	bool   ReloadShader(GLenum type, GLuint& id);
	void   DeleteShader(GLenum type, GLuint& id);
	void   LinkProgram();
	void   DeleteProgram();
public:
	Shader(const std::string& path);
	virtual ~Shader();

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

	Shader& operator->() const { return *shader; }
};

} // CUBE