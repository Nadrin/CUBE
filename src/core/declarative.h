/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#define IMPLEMENT_DEMO(DemoClass) void Core::Demo::Implementation() { DemoClass().Main(); }

#define render auto RenderFunction = [&](float Time)
#define commit System->Run(RenderFunction)