/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

// Main demo definition
#define IMPLEMENT_DEMO(DemoClass) void Core::Demo::Implementation() { DemoClass().Main(); }

// Blocks
#define render auto RenderFunction = [&](float Time)

// Commands
#define commit System->Run(RenderFunction)
#define param(type, var, group, ...) Parameter _param_##var(var, Parameter::type, group"/"#var, Parameter::Default, ##__VA_ARGS__)

// Group names
#define g_none   ""
#define g_global "Global"
#define g_scene  "Scene"
