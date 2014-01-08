/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/global.h>
#include <core/demo.h>

#include <demos/test.h>

using namespace CUBE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Global::Init();
	TestDemo().Run();
	Global::Terminate();
	return 0;
}