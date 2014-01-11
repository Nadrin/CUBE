/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/ui.h>

using namespace CUBE;
using namespace CUBE::Core;

#ifdef _DEBUG

TweakBarUI::TweakBarUI(GLFWwindow* window, const int width, const int height) 
	: UI(), ClientWidth(width), ClientHeight(height)
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

	if(Active)
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

void TweakBarUI::ParseName(const std::string& name, std::string& varname, std::string& vargrp) const
{
	size_t separator = name.find_first_of('/', 0);
	if(separator == std::string::npos) {
		varname = name;
		vargrp  = std::string();
	}
	else {
		varname = name.substr(separator+1);
		vargrp  = name.substr(0, separator);
	}
}

void TweakBarUI::PlaceBar(TwBar* bar, const int maxIndex)
{
	struct BarInfo {
		int position[2];
		int size[2];
	};

	if(Placement == PlacementMode::None)
		return;

	BarInfo info, otherInfo;
	TwGetParam(bar, NULL, "size", TW_PARAM_INT32, 2, info.size);

	int stepSize = 0;
	for(int i=1; i<maxIndex; i++) {
		TwBar* other = TwGetBarByIndex(i);
		TwGetParam(other, NULL, "size", TW_PARAM_INT32, 2, otherInfo.size);

		switch(Placement) {
		case PlacementMode::Horizontal:
			if(otherInfo.size[1] > stepSize)
				stepSize = otherInfo.size[1];
			break;
		case PlacementMode::Vertical:
			if(otherInfo.size[0] > stepSize)
				stepSize = otherInfo.size[0];
			break;
		}
	}

	info.position[0] = Padding;
	info.position[1] = Padding;
	for(int i=1; i<maxIndex-1; i++) {
		TwBar* other = TwGetBarByIndex(i);

		TwGetParam(other, NULL, "position", TW_PARAM_INT32, 2, otherInfo.position);
		TwGetParam(other, NULL, "size", TW_PARAM_INT32, 2, otherInfo.size);

		switch(Placement) {
		case PlacementMode::Horizontal:
			info.position[0] += otherInfo.size[0] + Padding;
			if(info.position[0] + info.size[0] > ClientWidth) {
				info.position[0]  = Padding;
				info.position[1] += stepSize + Padding;
			}
			break;
		case PlacementMode::Vertical:
			info.position[1] += otherInfo.size[1] + Padding;
			if(info.position[1] + info.size[1] > ClientHeight) {
				info.position[0] += stepSize + Padding;
				info.position[1]  = Padding;
			}
			break;
		}
	}

	std::stringstream sdef;
	sdef << TwGetBarName(bar) << " position='" << info.position[0] << " " << info.position[1] << "'";
	TwDefine(sdef.str().c_str());
}

TwBar* TweakBarUI::AddBar(const std::string& name)
{
	TwBar* bar = TwNewBar(name.c_str());
	PlaceBar(bar, TwGetBarCount());
	return bar;
}

void TweakBarUI::AddVariable(TwBar* bar, const std::string& name, TwType type, void* data)
{
	AddVariable(bar, name, type, data, std::string());
}

void TweakBarUI::AddVariable(TwBar* bar, const std::string& name, TwType type, void* data, const std::string& def)
{
	std::stringstream buffer;
	buffer << def << " ";

	std::string varname, vargrp;
	ParseName(name, varname, vargrp);

	if(!vargrp.empty()) {
		buffer << "group='" << vargrp << "'";
	}
	TwAddVarRW(bar, varname.c_str(), type, data, buffer.str().c_str());
}

void TweakBarUI::AddSeparator(TwBar* bar, const std::string& name)
{
	AddSeparator(bar, name, std::string());
}

void TweakBarUI::AddSeparator(TwBar* bar, const std::string& name, const std::string& def)
{
	std::stringstream buffer;
	buffer << def << " ";

	std::string varname, vargrp;
	ParseName(name, varname, vargrp);

	if(!vargrp.empty()) {
		buffer << "group='" << vargrp << "'";
	}
	TwAddSeparator(bar, varname.c_str(), buffer.str().c_str());
}

void TweakBarUI::Clear()
{
	TwDeleteAllBars();
}

void TweakBarUI::Draw()
{
	if(Active)
		TwDraw();
}

void TweakBarUI::Arrange()
{
	for(int i=1; i<TwGetBarCount(); i++) {
		TwBar* bar = TwGetBarByIndex(i);
		PlaceBar(bar, i+1);
	}
}

#endif