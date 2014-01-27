/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/config.h>
#include <core/ui.h>
#include <core/declarative.h>

#include <utils/parameter.h>

#include <classes/shader.h>
#include <classes/mesh.h>
#include <classes/actor.h>

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
	Mesh simpleMesh("monkey.dae");

	quat rotation;
	param(Quat, rotation);

	render {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		block {
			UseShader(simpleShader);
			
			shader.object().SetCameraMatrix(
				glm::perspective(45.0f, 1.77f, 1.0f, 100.0f), 
				glm::lookAt(vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	
			MeshActor actor(simpleMesh);
			actor.position() = vec3(0.0f, 0.0f, 1.0f);
			actor.rotation() = rotation;
			DrawActor(actor);
		}
		return true;
	};
	commit;
}
