/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#ifdef _DEBUG
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

enum class Hint
{
	Defaults,
	WithMaterials,
};

} // CUBE