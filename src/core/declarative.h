/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

// Main demo definition
#define IMPLEMENT_DEMO(DemoClass) void Core::Demo::Implementation() { DemoClass().Main(); }

// Blocks
#define render auto RenderFunction = [&](float Time)

// Commands
#define commit System->Run(RenderFunction)

// Parameter definitions
#define param(type, var, ...)                Parameter _param_##var(var, Parameter::type, #var, Parameter::Default, ##__VA_ARGS__)
#define gparam(type, var, group, ...)        Parameter _param_##var(var, Parameter::type, #group"/"#var, Parameter::Default, ##__VA_ARGS__)
#define xparam(type, var, group, flags, ...) Parameter _param_##var(var, Parameter::type, #group"/"#var, flags, ##__VA_ARGS__)