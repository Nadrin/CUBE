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
		Shader* restoreShader = nullptr;
		const bool isActive   = shader->IsActive();

		if(location == -1) return;

		if(!isActive) {
			restoreShader = Shader::Current();
			gltry(glUseProgram(shader->program));
		}

		glCall();

		if(!isActive) {
			if(restoreShader) {
				gltry(glUseProgram(restoreShader->program));
			}
			else {
				gltry(glUseProgram(0));
			}
		}
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
	Uniform& operator=(const std::vector<float>& value)
	{
		UseAndSet([this, &value]() {
			gltry(glUniform1fv(location, value.size(), &value[0]));
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
	Uniform& operator=(const std::vector<vec2>& value)
	{
		UseAndSet([this, &value]() {
			gltry(glUniform2fv(location, value.size(), glm::value_ptr(value[0])));
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
	Uniform& operator=(const std::vector<vec3>& value)
	{
		UseAndSet([this, &value]() {
			gltry(glUniform3fv(location, value.size(), glm::value_ptr(value[0])));
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
	Uniform& operator=(const std::vector<vec4>& value)
	{
		UseAndSet([this, &value]() {
			gltry(glUniform4fv(location, value.size(), glm::value_ptr(value[0])));
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
