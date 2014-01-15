/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <utils/notify.h>
#include <utils/parameter.h>

#include <classes/shader.h>

using namespace CUBE;

std::string Shader::Prefix("shaders\\");

Shader::Shader(const std::string& path) 
	: notifyHandler(this), path(Prefix+path), vs(0), fs(0), gs(0)
{
	program = glCreateProgram();

	vs = CompileShader(GL_VERTEX_SHADER);
	fs = CompileShader(GL_FRAGMENT_SHADER);
	gs = CompileShader(GL_GEOMETRY_SHADER);

	if(vs) glAttachShader(program, vs);
	if(fs) glAttachShader(program, fs);
	if(gs) glAttachShader(program, gs);

	LinkProgram();
}

Shader::~Shader()
{
	DeleteProgram();
}

std::string Shader::GetShaderFilename(GLenum type) const
{
	switch(type) {
	case GL_VERTEX_SHADER:
		return path+"_vs.glsl";
	case GL_FRAGMENT_SHADER:
		return path+"_fs.glsl";
	case GL_GEOMETRY_SHADER:
		return path+"_gs.glsl";
	}
	throw std::runtime_error("GetShaderFilename: Unsupported shader type!");
}

GLuint Shader::CompileShader(GLenum type)
{
	const std::string filename = GetShaderFilename(type);

	std::ifstream file(filename.c_str());
	if(!file.is_open())
		return 0;

	GLuint shader = glCreateShader(type);
	{
		std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		const GLchar* inSource[] = { source.c_str() };
		const GLint inLength[]   = { source.length() };

		glShaderSource(shader, 1, inSource, inLength);
	}

	Core::System::Instance()->Log("Compiling shader: %s ...\n", filename.c_str());
	glCompileShader(shader);
#ifdef _DEBUG
	{
		GLchar* infoLog;
		GLsizei infoLogSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);

		if(infoLogSize > 0) {
			infoLog = new GLchar[infoLogSize];
			glGetShaderInfoLog(shader, infoLogSize, NULL, infoLog);
			Core::System::Instance()->Log("%s\n", infoLog);
			delete[] infoLog;
		}
	}
#endif

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(GL_TRUE != status) {
		glDeleteShader(shader);
		throw std::runtime_error("Shader compilation failed.");
	}

#ifdef _DEBUG
	Core::System::Instance()->ShaderNotify->RegisterHandler(filename, &notifyHandler);
#endif
	return shader;
}

bool Shader::ReloadShader(GLenum type, GLuint& id)
{
	glDetachShader(program, id);
	glDeleteShader(id);

	id = CompileShader(type);
	if(!id)
		return false;

	glAttachShader(program, id);
	LinkProgram();
	return true;
}

void Shader::DeleteShader(GLenum type, GLuint& id)
{
#ifdef _DEBUG
	Core::System::Instance()->ShaderNotify->UnregisterHandler(GetShaderFilename(type));
#endif

	glDetachShader(program, id);
	glDeleteShader(id);
	id = 0;
}

void Shader::LinkProgram()
{
	Core::System::Instance()->Log("Linking shader program: %s ...\n", path.c_str());
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

#ifdef _DEBUG
	if(GL_TRUE == status) {
		glValidateProgram(program);
		glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	}

	GLsizei infoLogSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
	if(infoLogSize > 0) {
		GLchar* infoLog = new GLchar[infoLogSize];
		glGetProgramInfoLog(program, infoLogSize, NULL, infoLog);
		Core::System::Instance()->Log("%s\n", infoLog);
		delete[] infoLog;
	}
#endif

	if(GL_TRUE != status) {
		DeleteProgram();
		throw std::runtime_error("Shader program linking failed.");
	}
}

void Shader::DeleteProgram()
{
	if(vs) DeleteShader(GL_VERTEX_SHADER, vs);
	if(fs) DeleteShader(GL_FRAGMENT_SHADER, fs);
	if(gs) DeleteShader(GL_GEOMETRY_SHADER, gs);

	glDeleteProgram(program);
	program=0;
}

void Shader::NotifyHandler::operator()(const std::string& filename)
{
	auto Suffix = [](const std::string& a, const std::string& b) {
		if(b.size() > a.size()) return false;
		return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
	};

	if(Suffix(filename, "_vs.glsl")) {
		if(shader->vs)
			shader->ReloadShader(GL_VERTEX_SHADER, shader->vs);
	}
	else if(Suffix(filename, "_fs.glsl")) {
		if(shader->fs)
			shader->ReloadShader(GL_FRAGMENT_SHADER, shader->fs);
	}
	else if(Suffix(filename, "_gs.glsl")) {
		if(shader->gs)
			shader->ReloadShader(GL_GEOMETRY_SHADER, shader->gs);
	}
	else {
		Core::System::Instance()->Log("Warning: Received shader reload event for unknown filename: %s\n", filename.c_str());
	}
}

UseShader::UseShader(Shader& s) : shader(&s)
{
	glUseProgram(shader->program);
}

UseShader::~UseShader()
{
	glUseProgram(0);
}
