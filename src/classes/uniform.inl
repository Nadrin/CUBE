/* CUBE demo toolkit by MasterM/Asenses */

class Uniform
{
private:
	GLint location;
	const Shader* shader;

	Uniform(const Shader* s, const GLint loc=-1)
		: shader(s), location(loc) {}

	void UseAndSet(std::function<void ()> glCall)
	{
		GLint previousProgram;
		bool  switchProgram = false;

		if(!shader->isActive) {
			gltry(glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram));
			switchProgram = previousProgram != shader->program;
			if(switchProgram)
				gltry(glUseProgram(shader->program));
		}

		glCall();

		if(switchProgram)
			gltry(glUseProgram(previousProgram));
	}
public:
	Uniform(const Uniform& other)
		: shader(other.shader), location(other.location) {}

	Uniform& operator=(const Uniform& other)
	{
		location = other.location;
		return *this;
	}

	Uniform& operator=(const int& value)
	{
		UseAndSet([this, &value]() { gltry(glUniform1i(location, (GLint)value)); });
		return *this;
	}
	Uniform& operator=(const float& value)
	{
		UseAndSet([this, &value]() { gltry(glUniform1f(location, (GLfloat)value)); });
		return *this;
	}
	Uniform& operator=(const vec2& value)
	{
		UseAndSet([this, &value]() { gltry(glUniform2f(location, value.x, value.y)); });
		return *this;
	}
	Uniform& operator=(const vec3& value)
	{
		UseAndSet([this, &value]() { gltry(glUniform3f(location, value.x, value.y, value.z)); });
		return *this;
	}
	Uniform& operator=(const vec4& value)
	{
		UseAndSet([this, &value]() { gltry(glUniform4f(location, value.x, value.y, value.z, value.w)); });
		return *this;
	}

	friend class Shader;
};