/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/config.h>
#include <core/ui.h>

#include <utils/notify.h>

#include <cstdio>
#include <cstdarg>

using namespace CUBE;

class Core::System* CUBE::System = nullptr;
class Core::Config* CUBE::Config = nullptr;
class Core::UI*     CUBE::UI     = nullptr;

using namespace CUBE::Core;

System::System()
	: UI(nullptr), NotifyService(nullptr), paused(false), window(NULL), stream(NULL)
{
	strcpy_s<100>(name, "CUBE");
}

Core::System* System::Instance()
{
	static System _this;
	return &_this;
}

void System::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
#ifdef _DEBUG
		if(mods & GLFW_MOD_SHIFT)
#endif
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

#ifdef _DEBUG
	switch(key) {
	case GLFW_KEY_F1:
		if(action == GLFW_PRESS) System::Instance()->ToggleUI();
		return;
	case GLFW_KEY_F2:
		if(action == GLFW_PRESS) System::Instance()->ArrangeUI(PlacementMode::Horizontal);
		return;
	case GLFW_KEY_F3:
		if(action == GLFW_PRESS) System::Instance()->ArrangeUI(PlacementMode::Vertical);
		return;
	case GLFW_KEY_SPACE:
		if(action == GLFW_PRESS) System::Instance()->TogglePlayback();
		return;
	case GLFW_KEY_S:
		if(mods & GLFW_MOD_CONTROL && action == GLFW_PRESS) {
			CUBE::Config->Write();
			return;
		}
		break;
	case GLFW_KEY_LEFT:
		if(mods & GLFW_MOD_CONTROL) {
			if(action == GLFW_PRESS || action == GLFW_REPEAT) System::Instance()->Seek(-0.5f);
			return;
		}
		break;
	case GLFW_KEY_RIGHT:
		if(mods & GLFW_MOD_CONTROL) {
			if(action == GLFW_PRESS || action == GLFW_REPEAT) System::Instance()->Seek(0.5f);
			return;
		}
		break;
	}

	((TweakBarUI*)System::Instance()->UI)->TranslateKeyEvent(key, action);
#endif
}

void System::UpdateDebugInfo()
{
	static double lastTime = 0.0;
	double time = glfwGetTime();

	if((time - lastTime) >= 0.1) {
		char debugInfo[256];

		lastTime = time;
		sprintf_s(debugInfo, "%s [ %05.1fs ] %s", name, System::GetTime(), paused?"(paused)":"");
		glfwSetWindowTitle(window, debugInfo);
	}
}

void System::Init()
{
	std::setlocale(LC_ALL, "en_US.UTF-8");
	strcpy_s<100>(System::name, "CUBE");

#ifdef _DEBUG
	AllocConsole();
	SetConsoleTitleA("Debug Console");
	CUBE::Config = new ConfigRW();
#else
	CUBE::Config = new Config();
#endif

	if(!glfwInit()) {
		System::Error("Failed to initialize GLFW library.");
		ExitProcess(1);
	}

	if(!BASS_Init(-1, 44100, 0, 0, NULL)) {
		System::Error("Failed to initialize BASS library.");
		glfwTerminate();
		ExitProcess(1);
	}

	CUBE::System = this;
	System::Log("CUBE demo toolkit initialized.\n");
}

void System::Terminate()
{
	BASS_Stop();
	BASS_Free();

	delete NotifyService;
	delete CUBE::UI;
	delete CUBE::Config;

	glfwTerminate();
	System::Log("CUBE demo toolkit terminated.\n");

#ifdef _DEBUG
	FreeConsole();
#endif
}

void System::UseOpenGL(const int major, const int minor)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void System::OpenStream(const char* path)
{
	if(path)
		stream = BASS_StreamCreateFile(FALSE, path, 0, 0, BASS_STREAM_PRESCAN);
	else
		stream = BASS_StreamCreate(44100, 2, 0, STREAMPROC_DUMMY, NULL);

	if(!stream) {
		System::Error("Failed to open stream.");
		System::Terminate();
		ExitProcess(1);
	}

	System::Log("Opened stream: %s\n", path?path:"(dummy)");
}

void System::OpenDisplay(const int width, const int height, bool fullscreen)
{
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, name, NULL, NULL);
	if(!window) {
		System::Error("Failed to initialize OpenGL context.");
		System::Terminate();
		ExitProcess(1);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, System::KeyCallback);

	glewExperimental = GL_TRUE;
	glewInit();

	System::Log("Opened display: OpenGL %s (%dx%d)\n", glGetString(GL_VERSION), width, height);
	System::Log("Renderer: %s %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));

#ifdef _DEBUG
	System::UI = new Core::TweakBarUI(window, width, height);
	System::Log("Debug GUI initialized.\n");
#else
	System::UI = new Core::NullUI();
#endif

	CUBE::UI = System::UI;

	ClearErrorGL();
	SetDefaults();
}

void System::SetName(const char* name)
{
	strcpy_s<100>(System::name, name);
}

float System::GetTime()
{
	return (float)BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
}

void System::Run(RenderBlock render)
{
	BASS_ChannelPlay(stream, TRUE);

	while(!glfwWindowShouldClose(window)) {
		if(!render(GetTime())) 
			break;

		UI->Draw();
#ifdef _DEBUG
		System::UpdateDebugInfo();
		if(NotifyService)
			NotifyService->ProcessEvents();
#endif
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void System::Error(const char* error)
{
	MessageBoxA(NULL, error, "CUBE", MB_ICONSTOP | MB_OK);
}

void System::Log(const char* fmt, ...)
{
	const size_t BUFSIZE = 8192;
	char buffer[BUFSIZE];

	va_list args;
	va_start(args, fmt);
	vsnprintf_s(buffer, BUFSIZE, fmt, args);
	va_end(args);

	DWORD written;
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), &written, NULL);
}

void System::GetDisplaySize(int& w, int& h)
{
	glfwGetFramebufferSize(window, &w, &h);
}

float System::GetDisplayAspect()
{
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	return float(w)/float(h);
}

void System::TogglePlayback()
{
	if(paused) {
		BASS_ChannelPlay(stream, FALSE);
		paused = false;
	}
	else {
		BASS_ChannelPause(stream);
		paused = true;
	}
}

void System::ToggleUI()
{
	UI->Toggle();
}

void System::ArrangeUI(PlacementMode placement)
{
	if(UI->IsActive()) {
		UI->Placement = placement;
		UI->Arrange();
	}
}

void System::Seek(const float delta)
{
	double position = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
	position += double(delta);
	BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, position), BASS_POS_BYTE);
}

bool System::SetContentDirectory(const std::string& path)
{
#ifdef _DEBUG
	if(NotifyService)
		return false;
	NotifyService = new FileNotify(path);
#endif
	contentDirectory = path;
	return true;
}

const std::string& System::GetContentDirectory() const
{
	return contentDirectory;
}

void System::HandleException(const std::exception& e)
{
	std::string message(e.what());
	try { BASS_Stop(); } catch(const std::exception&) {};

#ifdef _DEBUG
	message.append("\nDo you want to debug?");
	if(MessageBoxA(NULL, message.c_str(), "CUBE [Exception]", MB_ICONSTOP | MB_YESNO) == IDYES)
		throw;
#else
	MessageBoxA(NULL, message.c_str(), "CUBE [Exception]", MB_ICONSTOP | MB_OK);
#endif
}

void System::ClearErrorGL()
{
	while(glGetError() != GL_NO_ERROR);
}

void System::CheckErrorGL(const char* call, const char* file, const int line)
{
	static const std::map<GLenum, std::string> errorString = {
		{ GL_INVALID_ENUM, "INVALID_ENUM" },
		{ GL_INVALID_VALUE, "INVALID_VALUE" },
		{ GL_INVALID_OPERATION, "INVALID_OPERATION" },
		{ GL_STACK_OVERFLOW, "STACK_OVERFLOW" },
		{ GL_STACK_UNDERFLOW, "STACK_UNDERFLOW" },
		{ GL_OUT_OF_MEMORY, "OUT_OF_MEMORY" },
		{ GL_INVALID_FRAMEBUFFER_OPERATION, "INVALID_FRAMEBUFFER_OPERATION" },
		{ GL_TABLE_TOO_LARGE, "TABLE_TOO_LARGE" }
	};

	GLenum error;
	while((error = glGetError()) != GL_NO_ERROR) {
		System::Instance()->Log("Error: %s = %02x (%s)\n  at %s:%d\n",
			call, error, errorString.at(error).c_str(), file, line);
	}
}

void System::SetDefaults()
{
	/* Depth testing */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	/* Backface culling */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	/* Blending */
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Multisampling */
	glEnable(GL_MULTISAMPLE);
}
