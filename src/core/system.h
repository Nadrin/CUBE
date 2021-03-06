/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#define CUBE_LOG(fmt, ...) Core::System::Instance()->Log(fmt, ##__VA_ARGS__)

namespace CUBE {

class FileNotify;
enum class PlacementMode;

typedef std::function<bool (float, float)> RenderFunction;
typedef std::function<void ()> KeyHandler;

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
	 std::string contentDirectory;

	 std::map<int, KeyHandler> keyHandlers;

	 struct {
		 GLuint vao;
		 GLuint vbo;
	 } ScreenQuad;

	 mutable struct {
		 GLuint frameTimeQuery;
		 std::string SceneName;
	 } DebugInfo;

	 System();
private:
	 void UpdateDebugInfo(const GLuint64 frameTime);
	 void SetDefaults();

	 void CreateScreenQuadVAO();
	 void DeleteScreenQuadVAO();

	 static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
	 static System* Instance();
	 class  FileNotify* NotifyService;
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

	 void  RegisterKey(int key, KeyHandler handler);
	 void  UnregisterKey(int key);

	 void  DrawScreenQuad() const;

	 void  Run(RenderFunction renderFunction);

	 bool SetContentDirectory(const std::string& path);
	 const std::string& GetContentDirectory() const;

	 void Debug_SetSceneName(const char* name) const;

	 static void ClearErrorGL();
	 static void CheckErrorGL(const char* call, const char* file, const int line);
	 static void HandleException(const std::exception& e);
};

} // Global

extern class Core::System*  System;
extern class Core::Config*  Config;
extern class Core::UI*      UI;

} // CUBE