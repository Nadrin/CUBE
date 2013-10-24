/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Global
{
private:
	Global() { }

	static char        name[100];
	static bool        paused;
	static GLFWwindow* window;
	static HSTREAM     stream;

	static void UpdateDebugInfo();
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
	static void Init(const char* name);
	static void Terminate();

	static void Error(const char* error);
	static void Log(const char* fmt, ...);

	static void UseOpenGL(const int major, const int minor);

	static void OpenStream(const char* path);
	static void OpenDisplay(const int width, const int height, bool fullscreen);

	static void  GetDisplaySize(int& w, int& h);
	static float GetDisplayAspect();

	static float GetTime();
	static void  Seek(const float delta);
	static void  TogglePlayback();

	static int   Run();
};

} // CUBE