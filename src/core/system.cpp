/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/config.h>
#include <core/ui.h>

#include <utils/notify.h>

#ifdef _DEBUG
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#endif

using namespace CUBE;

class Core::System* CUBE::System = nullptr;
class Core::Config* CUBE::Config = nullptr;
class Core::UI*     CUBE::UI     = nullptr;

using namespace CUBE::Core;

#ifdef _DEBUG
struct AssimpLogStream : public Assimp::LogStream
{
	void write(const char* message)
	{
		System::Instance()->Log("Assimp: %s", message);
	}
};
#endif

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

		std::string sceneName;
		if(!DebugInfo.SceneName.empty())
			sceneName = DebugInfo.SceneName + ": ";

		lastTime = time;
		sprintf_s(debugInfo, "%s [ %s%05.1fs ] %s", name,
			sceneName.c_str(), System::GetTime(), paused?"(paused)":"");
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

	Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	Assimp::DefaultLogger::get()->attachStream(new AssimpLogStream,
		Assimp::Logger::Warn | Assimp::Logger::Err);
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

	ilInit();

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

	DeleteScreenQuadVAO();
	glfwTerminate();

	System::Log("CUBE demo toolkit terminated.\n");

#ifdef _DEBUG
	Assimp::DefaultLogger::kill();
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
	CreateScreenQuadVAO();
}

void System::SetName(const char* name)
{
	strcpy_s<100>(System::name, name);
}

float System::GetTime()
{
	return (float)BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
}

void System::Run(RenderBlock renderFunction)
{
	BASS_ChannelPlay(stream, TRUE);

	while(!glfwWindowShouldClose(window)) {
		if(!renderFunction(GetTime())) 
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

void System::Debug_SetSceneName(const char* name) const
{
#ifdef _DEBUG
	DebugInfo.SceneName = std::string(name);
#endif
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

	/* OpenIL setup */
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	/* Viewport */
	int viewWidth, viewHeight;
	glfwGetFramebufferSize(window, &viewWidth, &viewHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, viewWidth, viewHeight);
}

void System::CreateScreenQuadVAO()
{
	const GLchar vertexData[] = {
		 1,  1, 1, 1,
		-1,  1, 0, 1,
		 1, -1, 1, 0,
		-1, -1, 0, 0,
	};

	gltry(glGenVertexArrays(1, &ScreenQuad.vao));
	gltry(glBindVertexArray(ScreenQuad.vao));

	gltry(glGenBuffers(1, &ScreenQuad.vbo));
	gltry(glBindBuffer(GL_ARRAY_BUFFER, ScreenQuad.vbo));
	gltry(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW));

	gltry(glVertexAttribPointer(0, 2, GL_BYTE, GL_FALSE, 4, (const GLvoid*)0));
	gltry(glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, 4, (const GLvoid*)2));
	gltry(glEnableVertexAttribArray(0));
	gltry(glEnableVertexAttribArray(1));

	gltry(glBindVertexArray(0));
	gltry(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void System::DeleteScreenQuadVAO()
{
	gltry(glDeleteVertexArrays(1, &ScreenQuad.vao));
	gltry(glDeleteBuffers(1, &ScreenQuad.vbo));
}

void System::DrawScreenQuad() const
{
	gltry(glBindVertexArray(ScreenQuad.vao));
	gltry(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	gltry(glBindVertexArray(0));
}
