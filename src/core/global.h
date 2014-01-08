/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

typedef std::function<bool (float)> RenderingFunction;

class Global
{
private:
	Global() { }

	static char        name[100];
	static bool        paused;
	static bool        ui;
	static GLFWwindow* window;
	static HSTREAM     stream;

	static void UpdateDebugInfo();
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void UnicodeCharCallback(GLFWwindow* window, unsigned int character);
public:
	static void Init();
	static void Terminate();

	static void Error(const char* error);
	static void Log(const char* fmt, ...);

	static void UseOpenGL(const int major, const int minor);

	static void OpenStream(const char* path);
	static void OpenDisplay(const int width, const int height, bool fullscreen);

	static void  SetName(const char* name);
	static void  GetDisplaySize(int& w, int& h);
	static float GetDisplayAspect();

	static float GetTime();
	static void  Seek(const float delta);

	static void  TogglePlayback();
	static void  ToggleUI();

	static void  Run(RenderingFunction func);
};

} // CUBE