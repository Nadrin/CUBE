/* CUBE demo toolkit by MasterM/Asenses */

class Uniform
{
private:
	GLint location;
	const Shader* shader;
	class ShaderParameter* parameter;

	Uniform(const Shader* s, const GLint loc=-1)
		: shader(s), location(loc), parameter(nullptr) {}

	void UseAndSet(std::function<void ()> glCall)
	{
		GLint previousProgram;
		bool  switchProgram = false;

		if(location == -1) return;

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

	template<typename T> void UpdateParameter(const T& value)
	{
		if(parameter)
			parameter->GetRef<typename T>() = value;
	}
public:
	Uniform(const Uniform& other)
		: shader(other.shader), location(other.location), parameter(nullptr) {}

	Uniform& operator=(const Uniform& other)
	{
		location = other.location;
		return *this;
	}

	Uniform& operator=(const int& value)
	{
		UseAndSet([this, &value]() { 
			gltry(glUniform1i(location, (GLint)value));
			UpdateParameter(value);
		});
		return *this;
	}
	Uniform& operator=(const float& value)
	{
		UseAndSet([this, &value]() { 
			gltry(glUniform1f(location, (GLfloat)value));
			UpdateParameter(value);
		});
		return *this;
	}
	Uniform& operator=(const vec2& value)
	{
		UseAndSet([this, &value]() { 
			gltry(glUniform2fv(location, 1, glm::value_ptr(value)));
			UpdateParameter(value);
		});
		return *this;
	}
	Uniform& operator=(const vec3& value)
	{
		UseAndSet([this, &value]() { 
			gltry(glUniform3fv(location, 1, glm::value_ptr(value)));
			UpdateParameter(value);
		});
		return *this;
	}
	Uniform& operator=(const vec4& value)
	{
		UseAndSet([this, &value]() { 
			gltry(glUniform4fv(location, 1, glm::value_ptr(value)));
			UpdateParameter(value);
		});
		return *this;
	}
	Uniform& operator=(const mat4& value)
	{
		UseAndSet([this, &value]() { 
			gltry(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)));
		});
		return *this;
	}

	friend class Shader;
	friend class ShaderParameter;
};
