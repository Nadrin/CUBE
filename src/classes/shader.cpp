/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <utils/notify.h>
#include <utils/parameter.h>

#include <classes/shader.h>
#include <classes/texture.h>
#include <classes/framebuffer.h>

using namespace CUBE;

std::string Shader::Prefix("shaders\\");
std::string ImageShader::VertexShaderName("image");

Shader::Shader()
	: notifyHandler(this), nullUniform(this),
	  vs(0), fs(0), gs(0), cs(0)
{
	program = gltry(glCreateProgram());
}

Shader::Shader(const std::string& path) 
	: name(path), path(Prefix+path), notifyHandler(this), nullUniform(this),
	  vs(0), fs(0), gs(0), cs(0)
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
	default:
		assert(0);
	}
	return std::string();
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

	CUBE_LOG("Compiling shader: %s\n", filename.c_str());
	gltry(glCompileShader(shader));
#ifdef CUBE_DEBUG
	{
		std::string infoLog(GetInfoLog(shader, type));
		if(infoLog.find_first_not_of(" \t\n\r") != std::string::npos)
			CUBE_LOG("\n%s\n", infoLog.c_str());
	}
#endif

	GLint status;
	gltry(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
	if(GL_TRUE != status) {
		gltry(glDeleteShader(shader));
		throw std::runtime_error("Shader compilation failed.");
	}

#ifdef CUBE_DEBUG
	Core::System::Instance()->NotifyService->RegisterHandler(filename, &notifyHandler);
#endif
	return shader;
}

bool Shader::ReloadShader(GLenum type, GLuint& id)
{
	GLuint newId;
	try {
		newId = CompileShader(type);
		if(!newId)
			throw std::runtime_error("Cannot read shader source file.");
	}
	catch(std::runtime_error& error) {
		CUBE_LOG("Reload failed: %s\n", error.what());
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
#ifdef CUBE_DEBUG
	Core::System::Instance()->NotifyService->UnregisterHandler(GetShaderFilename(type));
#endif

	gltry(glDetachShader(program, id));
	gltry(glDeleteShader(id));
	id = 0;
}

void Shader::LinkProgram()
{
	CUBE_LOG("Linking shader program: %s\n", path.c_str());
	gltry(glLinkProgram(program));

	GLint status;
	gltry(glGetProgramiv(program, GL_LINK_STATUS, &status));

#ifdef CUBE_DEBUG
	if(GL_TRUE == status) {
		gltry(glValidateProgram(program));
		gltry(glGetProgramiv(program, GL_VALIDATE_STATUS, &status));
	}

	{
		std::string infoLog(GetInfoLog(program, GL_PROGRAM));
		if(infoLog.find_first_not_of(" \t\n\r") != std::string::npos)
			CUBE_LOG("\n%s\n", infoLog.c_str());
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
	if(cs) DeleteShader(GL_COMPUTE_SHADER, cs);

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
	else if(Suffix(filename, "_cs.glsl")) {
		if(shader->cs)
			shader->ReloadShader(GL_COMPUTE_SHADER, shader->cs);
	}
	else {
		CUBE_LOG("Warning: Received shader reload event for unknown filename: %s\n", filename.c_str());
	}
}

Shader::Uniform* Shader::GetUniform(const std::string& name) const
{
	auto it = uniformCache.find(name);
	if(it != uniformCache.end())
		return &it->second;

	GLint location = gltry(glGetUniformLocation(program, name.c_str()));
	if(location == -1)
		return &nullUniform;

	return &uniformCache.insert({name, Shader::Uniform(this, location)}).first->second;
}

Shader::Uniform& Shader::operator[](const std::string& name) const
{
	auto it = uniformCache.find(name);
	if(it != uniformCache.end())
		return it->second;

	GLint location = gltry(glGetUniformLocation(program, name.c_str()));
	if(location == -1)
		return nullUniform;

	return uniformCache.insert({name, Shader::Uniform(this, location)}).first->second;
}

void Shader::ValidateUniforms()
{
	globalTime     = glGetUniformLocation(program, "GlobalTime");
	cameraMatrix   = glGetUniformLocation(program, "CameraMatrix");
	cameraPosition = glGetUniformLocation(program, "CameraPosition");
	modelMatrix    = glGetUniformLocation(program, "ModelMatrix");
	normalMatrix   = glGetUniformLocation(program, "NormalMatrix");

	for(auto it=uniformCache.begin(); it!=uniformCache.end();) {
		GLint location = gltry(glGetUniformLocation(program, it->first.c_str()));

		if(location == -1) {
			CUBE_LOG("Warning: Removed shader uniform %s::%s\n", name.c_str(), it->first.c_str());
			uniformCache.erase(it++);
		}
		else {
			it->second.location = location;
			++it;
		}
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
		if(ptype == Parameter::Vec3 && namebuf[1] == 'v')
			ptype = Parameter::Dir3;
		if(ptype == Parameter::Vec4 && namebuf[1] == 'c')
			ptype = Parameter::Color4;

		uniformParameters.push_back(new ShaderParameter(this, namebuf, ptype));
	}
}

bool Shader::SetGlobalTime(float time) const
{
	if(globalTime != -1) {
		gltry(glUniform1f(globalTime, time));
		return true;
	}
	return false;
}

bool Shader::SetCameraMatrix(const mat4& matrix) const
{
	if(cameraMatrix != -1) {
		gltry(glUniformMatrix4fv(cameraMatrix, 1, GL_FALSE, glm::value_ptr(matrix)));
		return true;
	}
	return false;
}

bool Shader::SetCameraMatrix(const mat4& projection, const mat4& view) const
{
	if(cameraMatrix != -1) {
		mat4 matrix = projection * view;
		gltry(glUniformMatrix4fv(cameraMatrix, 1, GL_FALSE, glm::value_ptr(matrix)));
		return true;
	}
	return false;
}

bool Shader::SetModelMatrix(const mat4& matrix) const
{
	if(modelMatrix != -1) {
		gltry(glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(matrix)));
		if(normalMatrix != -1)
			gltry(glUniformMatrix4fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(matrix))));
		return true;
	}
	return false;
}

bool CUBE::Shader::SetCameraPosition(const vec3& position) const
{
	if(cameraPosition != -1) {
		gltry(glUniform3f(cameraPosition, position.x, position.y, position.z));
		return true;
	}
	return false;
}

bool Shader::IsActive() const
{
	return ActiveShader::Stack.Current() && ActiveShader::Stack.Current()->InstanceOf(this);
}

Shader* Shader::Current()
{
	if(ActiveShader::Stack.Current())
		return ActiveShader::Stack.Current()->ptr();
	return nullptr;
}

ImageShader::ImageShader(const std::string& path) : Shader()
{
	this->name = path;
	this->path = Prefix+path;

	vs = CompileShader(GL_VERTEX_SHADER);
	fs = CompileShader(GL_FRAGMENT_SHADER);

	if(vs) gltry(glAttachShader(program, vs));
	if(fs) gltry(glAttachShader(program, fs));

	LinkProgram();
}

ImageShader::~ImageShader()
{
	if(vs) DeleteShader(GL_VERTEX_SHADER, vs);
	if(fs) DeleteShader(GL_FRAGMENT_SHADER, fs);
}

std::string ImageShader::GetShaderFilename(GLenum type) const
{
	switch(type) {
	case GL_VERTEX_SHADER:
		return Prefix+VertexShaderName+"_vs.glsl";
	case GL_FRAGMENT_SHADER:
		return path+"_fs.glsl";
	default:
		assert(0);
	}
	return std::string();
}

void ImageShader::Draw(const BlendFunc& blendFunc)
{
	const GLboolean blendingEnabled = glIsEnabled(GL_BLEND);

	if(!blendingEnabled) {
		gltry(glEnable(GL_BLEND));
	}
	gltry(glBlendFunc(blendFunc.SourceFactor, blendFunc.DestFactor));
	
	{
		ActiveShader shader(*this);
		Core::System::Instance()->DrawScreenQuad();
	}

	gltry(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	if(!blendingEnabled) {
		gltry(glDisable(GL_BLEND));
	}
}

void ImageShader::Draw(Texture& input, const BlendFunc& blendFunc)
{
	ActiveTexture texture(0, input);
	Draw(blendFunc);
}

void ImageShader::Draw(Texture& input, FrameBuffer& output, const BlendFunc& blendFunc)
{
	ActiveTexture   texture(0, input);
	DrawFrameBuffer drawfb(output);
	Draw(blendFunc);
}

ComputeShader::ComputeShader(const std::string& path) : Shader()
{
	this->name = path;
	this->path = Prefix+path;

	cs = CompileShader(GL_COMPUTE_SHADER);
	if(cs) gltry(glAttachShader(program, cs));

	LinkProgram();
}

ComputeShader::~ComputeShader()
{
	if(cs) DeleteShader(GL_COMPUTE_SHADER, cs);
}

std::string ComputeShader::GetShaderFilename(GLenum type) const
{
	switch(type) {
	case GL_COMPUTE_SHADER:
		return path+"_cs.glsl";
	default:
		assert(0);
	}
	return std::string();
}

void ComputeShader::Dispatch(const Dim& groups) const
{
	gltry(glDispatchCompute(groups.GetWidth(), groups.GetHeight(), groups.GetDepth()));
}

void ComputeShader::DispatchSync(const Dim& groups, const GLenum barrier) const
{
	gltry(glDispatchCompute(groups.GetWidth(), groups.GetHeight(), groups.GetDepth()));
	gltry(glMemoryBarrier(barrier));
}

CUBE_STACK(ActiveShader);

ActiveShader::ActiveShader(Shader& s) : ActiveObject(s)
{
	CUBE_PUSH;
	gltry(glUseProgram(objectPtr->program));
	objectPtr->SetGlobalTime(Core::System::Instance()->GetTime());
}

ActiveShader::~ActiveShader()
{
	gltry(glUseProgram(0));
	CUBE_POP;
}
