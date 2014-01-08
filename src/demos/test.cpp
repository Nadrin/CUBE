/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/global.h>
#include <demos/test.h>

using namespace CUBE;

void TestDemo::Run()
{
	Global::SetName("CUBE: Test Demo");
	Global::UseOpenGL(4, 3);
	Global::OpenStream("music.mp3");
	Global::OpenDisplay(1920, 1080, false);

	render {
		glClear(GL_COLOR_BUFFER_BIT);
		return true;
	};
	commit;
}
