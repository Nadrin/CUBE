/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/global.h>

using namespace CUBE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Global::Init("CUBE");
	Global::UseOpenGL(4, 3);
	Global::OpenStream("music.mp3");
	Global::OpenDisplay(1920, 1080, false);

	return Global::Run();
}
