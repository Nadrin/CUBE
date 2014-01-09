/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/demo.h>

using namespace CUBE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	using System = Core::System;

	System::Instance()->Init();
	Core::Demo::Implementation();
	System::Instance()->Terminate();
	return 0;
}