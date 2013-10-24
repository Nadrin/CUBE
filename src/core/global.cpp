/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/global.h>

#include <cstdio>
#include <cstdarg>

using namespace CUBE;

bool        Global::paused    = false;
char        Global::name[100] = {0};
GLFWwindow* Global::window    = NULL;
HSTREAM     Global::stream    = NULL;

void Global::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

#ifdef _DEBUG
	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		Global::TogglePlayback();
	if(key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		Global::Seek(-0.5f);
	if(key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		Global::Seek(0.5f);
#endif
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

void Global::Init(const char* name)
{
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

	strcpy_s(Global::name, name);
	Global::Log("CUBE demo toolkit initialized.\n");
}

void Global::Terminate()
{
	BASS_Stop();
	BASS_Free();

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

	Global::Log("Opened display: OpenGL %s (%dx%d)\n", glGetString(GL_VERSION), width, height);
	Global::Log("Renderer: %s %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
}

float Global::GetTime()
{
	return (float)BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
}

int Global::Run()
{
	BASS_ChannelPlay(stream, TRUE);

	while(!glfwWindowShouldClose(window)) {
#ifdef _DEBUG
		Global::UpdateDebugInfo();
#endif

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Global::Terminate();
	return 0;
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

void Global::Seek(const float delta)
{
	double position = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
	position += double(delta);
	BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, position), BASS_POS_BYTE);
}