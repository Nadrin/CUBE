/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <clocale>
#include <exception>
#include <string>
#include <sstream>
#include <fstream>
#include <queue>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <memory>
#include <functional>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <IL/il.h>
#include <GLFW/glfw3.h>
#include <bass.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <AntTweakBar.h>

namespace CUBE {

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4x4;
using quat = glm::fquat;

} // CUBE