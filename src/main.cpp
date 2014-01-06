/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/global.h>
#include <utils/notify.h>

using namespace CUBE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Global::Init("CUBE");
	Global::UseOpenGL(4, 3);
	Global::OpenStream("music.mp3");
	Global::OpenDisplay(1920, 1080, false);

	struct TestHandler : public FileNotify::Handler {
		virtual void operator()() override 
		{
			Global::Log("EXEC!\n");
		}
	} TestHandlerObject;

	FileNotify notify("\\shaders");
	notify.RegisterHandler("test.txt", &TestHandlerObject);

	return Global::Run([&](float time)
	{
		notify.ProcessEvents();
		return true;
	});
}
