/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class FileNotify;
enum class PlacementMode;

typedef std::function<bool (float)> RenderBlock;

namespace Core {

class UI;
class Config;

class System
{
private:
	 char        name[100];
	 bool        paused;
	 GLFWwindow* window;
	 HSTREAM     stream;

	 System();
private:
	 void UpdateDebugInfo();
	 static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
	 static System* Instance();
	 class  FileNotify* ShaderNotify;
	 class  UI* UI;
public:
	 void Init();
	 void Terminate();

	 void Error(const char* error);
	 void Log(const char* fmt, ...);

	 void UseOpenGL(const int major, const int minor);

	 void OpenStream(const char* path);
	 void OpenDisplay(const int width, const int height, bool fullscreen);

	 void  SetName(const char* name);
	 void  GetDisplaySize(int& w, int& h);
	 float GetDisplayAspect();

	 float GetTime();
	 void  Seek(const float delta);

	 void  TogglePlayback();
	 void  ToggleUI();
	 void  ArrangeUI(PlacementMode placement);

	 void  ShaderDirectory(const std::string& path);

	 void  Run(RenderBlock render);
};

} // Global

extern class Core::System* System;
extern class Core::Config* Config;
extern class Core::UI*     UI;

} // CUBE