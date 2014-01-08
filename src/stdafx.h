/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <clocale>
#include <exception>
#include <string>
#include <queue>
#include <map>
#include <functional>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <bass.h>
#include <glm/glm.hpp>

#ifdef _DEBUG
#include <AntTweakBar.h>
#endif