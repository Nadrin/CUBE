/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/global.h>

#include <cstdio>
#include <cstdarg>

using namespace CUBE;

bool        Global::paused    = false;
bool		Global::ui        = false;
char        Global::name[100] = {0};
GLFWwindow* Global::window    = NULL;
HSTREAM     Global::stream    = NULL;

void Global::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

#ifdef _DEBUG
	switch(key) {
	case GLFW_KEY_F1:
		if(action == GLFW_PRESS)
			Global::ToggleUI();
		return;
	case GLFW_KEY_SPACE:
		if(action == GLFW_PRESS)
			Global::TogglePlayback();
		return;
	case GLFW_KEY_LEFT:
		if(mods & GLFW_MOD_CONTROL) {
			if(action == GLFW_PRESS || action == GLFW_REPEAT)
				Global::Seek(-0.5f);
			return;
		}
		break;
	case GLFW_KEY_RIGHT:
		if(mods & GLFW_MOD_CONTROL) {
			if(action == GLFW_PRESS || action == GLFW_REPEAT)
				Global::Seek(0.5f);
			return;
		}
		break;
	}

	// AntTweakBar compatibility fixes

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

	if(Global::ui)
		TwEventKeyGLFW(tw_key, tw_action);
#endif
}

void Global::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if(Global::ui)
		TwEventMouseButtonGLFW(button, action);
}

void Global::MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if(Global::ui)
		TwEventMousePosGLFW(int(xpos), int(ypos));
}

void Global::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(Global::ui)
		TwEventMouseWheelGLFW(int(yoffset));
}

void Global::UnicodeCharCallback(GLFWwindow* window, unsigned int character)
{
	if(Global::ui)
		TwEventCharGLFW(character, GLFW_PRESS);
}

void Global::UpdateDebugInfo()
{
	static double lastTime = 0.0;
	double time = glfwGetTime();

	if((time - lastTime) >= 0.1) {
		char debugInfo[256];

		lastTime = time;
		sprintf_s(debugInfo, "%s [ %05.1fs ] %s", name, Global::GetTime(), paused?"(paused)":"");
		glfwSetWindowTitle(window, debugInfo);
	}
}

void Global::Init()
{
	std::setlocale(LC_ALL, "en_US.UTF-8");
	strcpy_s<100>(Global::name, "CUBE");

#ifdef _DEBUG
	AllocConsole();
	SetConsoleTitleA("Debug Console");
#endif

	if(!glfwInit()) {
		Global::Error("Failed to initialize GLFW library.");
		ExitProcess(1);
	}

	if(!BASS_Init(-1, 44100, 0, 0, NULL)) {
		Global::Error("Failed to initialize BASS library.");
		glfwTerminate();
		ExitProcess(1);
	}

	Global::Log("CUBE demo toolkit initialized.\n");
}

void Global::Terminate()
{
	BASS_Stop();
	BASS_Free();

#ifdef _DEBUG
	TwTerminate();
#endif

	glfwTerminate();
	Global::Log("CUBE demo toolkit terminated.\n");

#ifdef _DEBUG
	FreeConsole();
#endif
}

void Global::UseOpenGL(const int major, const int minor)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Global::OpenStream(const char* path)
{
	if(path)
		stream = BASS_StreamCreateFile(FALSE, path, 0, 0, BASS_STREAM_PRESCAN);
	else
		stream = BASS_StreamCreate(44100, 2, 0, STREAMPROC_DUMMY, NULL);

	if(!stream) {
		Global::Error("Failed to open stream.");
		Global::Terminate();
		ExitProcess(1);
	}

	Global::Log("Opened stream: %s\n", path?path:"(dummy)");
}

void Global::OpenDisplay(const int width, const int height, bool fullscreen)
{
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, name, NULL, NULL);
	if(!window) {
		Global::Error("Failed to initialize OpenGL context.");
		Global::Terminate();
		ExitProcess(1);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, Global::KeyCallback);

	glewExperimental = GL_TRUE;
	glewInit();

#ifdef _DEBUG
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(width, height);
	glfwSetMouseButtonCallback(window, Global::MouseButtonCallback);
	glfwSetScrollCallback(window, Global::MouseScrollCallback);
	glfwSetCursorPosCallback(window, Global::MousePositionCallback);
	glfwSetCharCallback(window, Global::UnicodeCharCallback);
#endif

	Global::Log("Opened display: OpenGL %s (%dx%d)\n", glGetString(GL_VERSION), width, height);
	Global::Log("Renderer: %s %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
}

void Global::SetName(const char* name)
{
	strcpy_s<100>(Global::name, name);
}

float Global::GetTime()
{
	return (float)BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
}

void Global::Run(RenderingFunction func)
{
	BASS_ChannelPlay(stream, TRUE);

	while(!glfwWindowShouldClose(window)) {
		if(!func(GetTime()))
			break;

#ifdef _DEBUG
		Global::UpdateDebugInfo();
		if(Global::ui)
			TwDraw();
#endif
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Global::Error(const char* error)
{
	MessageBoxA(NULL, error, "CUBE", MB_ICONSTOP | MB_OK);
}

void Global::Log(const char* fmt, ...)
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

void Global::GetDisplaySize(int& w, int& h)
{
	glfwGetFramebufferSize(window, &w, &h);
}

float Global::GetDisplayAspect()
{
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	return float(w)/float(h);
}

void Global::TogglePlayback()
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

void Global::ToggleUI()
{
	Global::ui = !Global::ui;
}

void Global::Seek(const float delta)
{
	double position = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
	position += double(delta);
	BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, position), BASS_POS_BYTE);
}