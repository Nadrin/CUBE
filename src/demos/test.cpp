/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/config.h>
#include <core/ui.h>
#include <core/declarative.h>

#include <utils/parameter.h>

#include <classes/shader.h>

#include <demos/test.h>

using namespace CUBE;

IMPLEMENT_DEMO(TestDemo)

void TestDemo::Main()
{
	System->SetName("CUBE: Test Demo");
	System->UseOpenGL(4, 3);
	System->OpenDisplay(1280, 720, false);
	System->OpenStream("music.mp3");

	System->SetContentDirectory(".");
	Config->Read("demo.ini");

	Shader simpleShader("simple");
	simpleShader["test"] = vec4(0.5f);

	render {
		scene(Scene1) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			block {
				UseShader shader(simpleShader);
			}
		};

		sequence {
			play(Scene1, 5.0f);
			quit;
		}

		return true;
	};
	commit;
}
