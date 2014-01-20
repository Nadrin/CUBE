/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <utils/notify.h>
#include <utils/parameter.h>

#include <classes/shader.h>

using namespace CUBE;

std::string Shader::Prefix("shaders\\");

Shader::Shader(const std::string& path) 
	: name(path), path(Prefix+path), notifyHandler(this), isActive(false), vs(0), fs(0), gs(0)
{
	program = gltry(glCreateProgram());

	vs = CompileShader(GL_VERTEX_SHADER);
	fs = CompileShader(GL_FRAGMENT_SHADER);
	gs = CompileShader(GL_GEOMETRY_SHADER);

	if(vs) gltry(glAttachShader(program, vs));
	if(fs) gltry(glAttachShader(program, fs));
	if(gs) gltry(glAttachShader(program, gs));

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

std::string Shader::GetInfoLog(const GLuint id, GLenum type) const
{
	GLsizei infoLogSize;

	if(type == GL_PROGRAM) {
		gltry(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogSize));
		if(infoLogSize > 0) {
			std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogSize]);
			gltry(glGetProgramInfoLog(id, infoLogSize, NULL, infoLog.get()));
			return std::string(infoLog.get());
		}
	}
	else {
		gltry(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogSize));
		if(infoLogSize > 0) {
			std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogSize]);
			gltry(glGetShaderInfoLog(id, infoLogSize, NULL, infoLog.get()));
			return std::string(infoLog.get());
		}
	}
	return std::string();
}

GLuint Shader::CompileShader(GLenum type)
{
	const std::string filename = GetShaderFilename(type);

	std::ifstream file(filename.c_str());
	if(!file.is_open())
		return 0;

	GLuint shader = gltry(glCreateShader(type));
	{
		std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		const GLchar* inSource[] = { source.c_str() };
		const GLint inLength[]   = { source.length() };

		gltry(glShaderSource(shader, 1, inSource, inLength));
	}

	Core::System::Instance()->Log("Compiling shader: %s ...\n", filename.c_str());
	gltry(glCompileShader(shader));
#ifdef _DEBUG
	{
		std::string infoLog(GetInfoLog(shader, type));
		if(infoLog.find_first_not_of(" \t\n\r") != std::string::npos)
			Core::System::Instance()->Log("\n%s\n", infoLog.c_str());
	}
#endif

	GLint status;
	gltry(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
	if(GL_TRUE != status) {
		gltry(glDeleteShader(shader));
		throw std::runtime_error("Shader compilation failed.");
	}

#ifdef _DEBUG
	Core::System::Instance()->NotifyService->RegisterHandler(filename, &notifyHandler);
#endif
	return shader;
}

bool Shader::ReloadShader(GLenum type, GLuint& id)
{
	Core::System::Instance()->Log("Received shader reload notification.\n");
	
	GLuint newId;
	try {
		newId = CompileShader(type);
		if(!newId)
			throw std::runtime_error("Cannot read shader source file.");
	}
	catch(std::runtime_error& error) {
		Core::System::Instance()->Log("Reload failed: %s\n", error.what());
		return false;
	}

	gltry(glDetachShader(program, id));
	gltry(glDeleteShader(id));

	id = newId;
	gltry(glAttachShader(program, id));
	LinkProgram();
	return true;
}

void Shader::DeleteShader(GLenum type, GLuint& id)
{
#ifdef _DEBUG
	Core::System::Instance()->NotifyService->UnregisterHandler(GetShaderFilename(type));
#endif

	gltry(glDetachShader(program, id));
	gltry(glDeleteShader(id));
	id = 0;
}

void Shader::LinkProgram()
{
	Core::System::Instance()->Log("Linking shader program: %s ...\n", path.c_str());
	gltry(glLinkProgram(program));

	GLint status;
	gltry(glGetProgramiv(program, GL_LINK_STATUS, &status));

#ifdef _DEBUG
	if(GL_TRUE == status) {
		gltry(glValidateProgram(program));
		gltry(glGetProgramiv(program, GL_VALIDATE_STATUS, &status));
	}

	{
		std::string infoLog(GetInfoLog(program, GL_PROGRAM));
		if(infoLog.find_first_not_of(" \t\n\r") != std::string::npos)
			Core::System::Instance()->Log("\n%s\n", infoLog.c_str());
	}
#endif

	if(GL_TRUE != status) {
		DeleteProgram();
		throw std::runtime_error("Shader program linking failed.");
	}

	ValidateUniforms();
	CreateParameters();
}

void Shader::DeleteProgram()
{
	uniformParameters.remove_if([](ShaderParameter* p) { delete p; return true; });
	uniformCache.clear();

	if(vs) DeleteShader(GL_VERTEX_SHADER, vs);
	if(fs) DeleteShader(GL_FRAGMENT_SHADER, fs);
	if(gs) DeleteShader(GL_GEOMETRY_SHADER, gs);

	gltry(glDeleteProgram(program));
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

Shader::Uniform& Shader::operator[](const std::string& name) const
{
	auto it = uniformCache.find(name);
	if(it != uniformCache.end())
		return it->second;

	GLint location = gltry(glGetUniformLocation(program, name.c_str()));
	assert(location != -1);

	return uniformCache.insert({name, Shader::Uniform(this, location)}).first->second;
}

void Shader::ValidateUniforms()
{
	for(auto kv : uniformCache) {
		GLint location = gltry(glGetUniformLocation(program, kv.first.c_str()));
		if(location == -1)
			throw std::runtime_error("Cannot validate shader uniforms.");
		kv.second.location = location;
	}
}

void Shader::CreateParameters()
{
	uniformParameters.remove_if([](ShaderParameter* p) { delete p; return true; });

	GLint  count;
	GLchar namebuf[256];
	gltry(glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count));

	for(GLint i=0; i<count; i++) {
		GLint size;
		GLenum type;
		GLsizei namelen;
		Parameter::Type ptype;

		gltry(glGetActiveUniform(program, i, 256, &namelen, &size, &type, namebuf));

		if(size != 1 || namelen < 3 || namebuf[0] != 'p')
			continue;

		switch(type) {
		case GL_INT:   ptype = Parameter::Int; break;
		case GL_FLOAT: ptype = Parameter::Float; break;
		case GL_FLOAT_VEC2: ptype = Parameter::Vec2; break;
		case GL_FLOAT_VEC3: ptype = Parameter::Vec3; break;
		case GL_FLOAT_VEC4: ptype = Parameter::Vec4; break;
		default: continue;
		}

		if(ptype == Parameter::Vec3 && namebuf[1] == 'c')
			ptype = Parameter::Color3;
		if(ptype == Parameter::Vec3 && namebuf[1] == 'd')
			ptype = Parameter::Direction;
		if(ptype == Parameter::Vec4 && namebuf[1] == 'c')
			ptype = Parameter::Color4;

		uniformParameters.push_back(new ShaderParameter(this, namebuf, ptype));
	}
}

UseShader::UseShader(Shader& s) : shader(&s)
{
	gltry(glUseProgram(shader->program));
	shader->isActive = true;
}

UseShader::~UseShader()
{
	gltry(glUseProgram(0));
	shader->isActive = false;
}
