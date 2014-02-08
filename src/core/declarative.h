/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

// Main demo definition
#define IMPLEMENT_DEMO(DemoClass) void Core::Demo::Implementation() { DemoClass().Main(); }

// Blocks
#define block          // Syntactic sugar for simple C++ scope block
#define sequence       Declarative::Sequence _Sequence(Time);
#define scene(name)    static auto _SceneFunction_##name = [&](float SceneTime, float SceneDuration)
#define render         static auto _RenderFunction = [&](float Time)

// Sequence control
#define play(name, duration) {                                     \
	if(_Sequence.ShouldPlay(duration)) {                           \
		_SceneFunction_##name(_Sequence.GetSceneTime(), duration); \
		System->Debug_SetSceneName(#name);                         \
	}                                                              \
	_Sequence.Advance(duration);                                   \
}
#define quit   {               \
	if(_Sequence.ShouldQuit()) \
		return false;          \
}
#define commit System->Run(_RenderFunction)

// Commands
#define UseShader(shaderObject)               ActiveShader shader(shaderObject)
#define UseCamera(cameraActor)                ActiveCamera camera(cameraActor)
#define UseMaterial(materialObject)           ActiveMaterial material(materialObject)
#define UseTexture(unit, textureObject, ...)  ActiveTexture texture##unit(unit, textureObject, ##__VA_ARGS__)
#define UseDrawBuffer(fbObject)				  DrawFrameBuffer drawfb(fbObject)
#define UseReadBuffer(fbObject)               ReadFrameBuffer readfb(fbObject)
#define DrawActor(actor)                      actor.Draw()

// Parameter definitions
#define param(type, var, ...)                Parameter _param_##var(var, Parameter::type, #var, Parameter::Default, ##__VA_ARGS__)
#define gparam(type, var, group, ...)        Parameter _param_##var(var, Parameter::type, #group"/"#var, Parameter::Default, ##__VA_ARGS__)
#define xparam(type, var, group, flags, ...) Parameter _param_##var(var, Parameter::type, #group"/"#var, flags, ##__VA_ARGS__)

// Helper classes
namespace CUBE {
namespace Declarative {

class Sequence
{
private:
	const float& time;
	float state;

public:
	Sequence(const float& t) : time(t), state(0.0f) {}

	float GetSceneTime() const
	{
		return glm::max(time - state, 0.0f);
	}
	bool ShouldPlay(const float duration) const
	{
		return time >= state && time < state + duration;
	}
	bool ShouldQuit() const
	{
		return time > state;
	}
	void Advance(const float duration)
	{
		state += duration;
	}
};

} // Declarative
} // CUBE