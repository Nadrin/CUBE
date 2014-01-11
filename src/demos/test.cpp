/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/ui.h>
#include <core/declarative.h>

#include <demos/test.h>

using namespace CUBE;

IMPLEMENT_DEMO(TestDemo)

void TestDemo::Main()
{
	System->SetName("CUBE: Test Demo");
	System->UseOpenGL(4, 3);
	System->OpenDisplay(1920, 1080, false);
	System->OpenStream("music.mp3");

	UI->Placement = PlacementMode::Horizontal;

	render {
		glClear(GL_COLOR_BUFFER_BIT);
		return true;
	};
	commit;
}
