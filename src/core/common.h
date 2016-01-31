/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#define CUBE_DEBUG_PRINTFREQ 0.1

#define CUBE_MAX_ATTACHMENTS 16
#define CUBE_MAX_BINDINGS    32

#ifdef CUBE_DEBUG_GL
#define gltry(function_call) \
	function_call; \
	CUBE::Core::System::CheckErrorGL(#function_call, __FILE__, __LINE__)
#else
#define gltry(function_call) function_call
#endif

namespace CUBE {

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4x4;
using quat = glm::fquat;

static const float Epsilon = 0.00001f;
static const vec3  UpVector(0.0f, 1.0f, 0.0f);
static const vec3  ForwardVector(0.0f, 0.0f, 1.0f);

typedef unsigned int Flags;

namespace Hint
{
	enum Type
	{
		Defaults      = 0x00,
		WithMaterials = 0x01,
		FlatNormals   = 0x02,
	};
}

enum LoadFromFile   { FromFile   };
enum LoadFromString { FromString };
enum LoadFromMemory { FromMemory };

struct BlendFunc
{
	GLenum SourceFactor;
	GLenum DestFactor;

	BlendFunc(const GLenum src=GL_SRC_ALPHA, const GLenum dst=GL_ONE_MINUS_SRC_ALPHA)
	{
		SourceFactor = src;
		DestFactor   = dst;
	}
};

template<class T>
class ActiveObject
{
protected:
	T* objectPtr;

	ActiveObject(T& ref) : objectPtr(&ref)
	{}
public:
	T* operator->() const
	{
		return objectPtr;
	}
	T* ptr() const
	{
		return objectPtr;
	}
	T& object() const
	{
		return *objectPtr;
	}

	bool InstanceOf(const T* other) const
	{
		return objectPtr == other;
	}
};

} // CUBE