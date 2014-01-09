/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/ui.h>

using namespace CUBE;
using namespace CUBE::Core;

#ifdef _DEBUG

TweakBarUI::TweakBarUI(GLFWwindow* window, const int width, const int height) : UI()
{
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(width, height);

	glfwSetMouseButtonCallback(window, TweakBarUI::MouseButtonCallback);
	glfwSetScrollCallback(window, TweakBarUI::MouseScrollCallback);
	glfwSetCursorPosCallback(window, TweakBarUI::MousePositionCallback);
	glfwSetCharCallback(window, TweakBarUI::UnicodeCharCallback);
}

TweakBarUI::~TweakBarUI()
{
	TwTerminate();
}

void TweakBarUI::TranslateKeyEvent(int key, int action)
{
	int tw_key, tw_action;
	switch(key) {
	case GLFW_KEY_UP:        tw_key = 256+27; break;
	case GLFW_KEY_DOWN:      tw_key = 256+28; break;
	case GLFW_KEY_LEFT:      tw_key = 256+29; break;
	case GLFW_KEY_RIGHT:     tw_key = 256+30; break;
	case GLFW_KEY_TAB:       tw_key = 256+37; break;
	case GLFW_KEY_ENTER:     tw_key = 256+38; break;
	case GLFW_KEY_BACKSPACE: tw_key = 256+39; break;
	case GLFW_KEY_INSERT:    tw_key = 256+40; break;
	case GLFW_KEY_DELETE:    tw_key = 256+41; break;
	case GLFW_KEY_PAGE_UP:   tw_key = 256+42; break;
	case GLFW_KEY_PAGE_DOWN: tw_key = 256+43; break;
	case GLFW_KEY_HOME:      tw_key = 256+44; break;
	case GLFW_KEY_END:       tw_key = 256+45; break;

	default: tw_key = key;
	}

	if(action == GLFW_REPEAT) tw_action = GLFW_PRESS;
	else tw_action = action;

	if(active)
		TwEventKeyGLFW(tw_key, tw_action);
}

void TweakBarUI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if(System::Instance()->UI->IsActive())
		TwEventMouseButtonGLFW(button, action);
}

void TweakBarUI::MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if(System::Instance()->UI->IsActive())
		TwEventMousePosGLFW(int(xpos), int(ypos));
}

void TweakBarUI::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(System::Instance()->UI->IsActive())
		TwEventMouseWheelGLFW(int(yoffset));
}

void TweakBarUI::UnicodeCharCallback(GLFWwindow* window, unsigned int character)
{
	if(System::Instance()->UI->IsActive())
		TwEventCharGLFW(character, GLFW_PRESS);
}

void TweakBarUI::Draw()
{
	if(active)
		TwDraw();
}

TwBar* TweakBarUI::AddBar(const std::string& name)
{
	return nullptr;
}

void TweakBarUI::AddVariable(TwBar* bar, const std::string& name, TwType type, void* data)
{
}

void TweakBarUI::AddSeparator(TwBar* bar, const std::string& name)
{
}

#endif