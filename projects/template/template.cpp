/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>

#include <core/system.h>
#include <core/config.h>
#include <core/demo.h>
#include <core/declarative.h>

using namespace CUBE;

class TemplateDemo : public Core::Demo
{
public:
	void Main();
};

IMPLEMENT_DEMO(TemplateDemo)

void TemplateDemo::Main()
{
	System->SetName("CUBE: Template Demo");
	System->UseOpenGL(4, 4);
	System->OpenDisplay(1280, 720, false);
	System->OpenStream("blank.ogg");

	System->SetContentDirectory(".");
	Config->Read("demo.ini");

	render {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	};
	commit;
}