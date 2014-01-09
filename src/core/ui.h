/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {
namespace Core {

class UI
{
protected:
	bool active;

	UI() : active(false) {}
public:
	virtual ~UI() {}

	bool IsActive() const { return active;    }
	void Toggle()         { active = !active; }

	virtual TwBar* AddBar(const std::string& name) { return nullptr; }
	virtual void   AddVariable(TwBar* bar, const std::string& name, TwType type, void* data) {}
	virtual void   AddSeparator(TwBar* bar, const std::string& name) {}

	virtual void   Draw() {}
};

class NullUI : public UI
{
public:
	NullUI() : UI() {}
};

#ifdef _DEBUG
class TweakBarUI : public UI
{
protected:
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void UnicodeCharCallback(GLFWwindow* window, unsigned int character);
public:
	TweakBarUI(GLFWwindow* window, const int width, const int height);
	virtual ~TweakBarUI();

	void TranslateKeyEvent(int key, int action);

	virtual TwBar* AddBar(const std::string& name) override;
	virtual void   AddVariable(TwBar* bar, const std::string& name, TwType type, void* data) override;
	virtual void   AddSeparator(TwBar* bar, const std::string& name) override;

	virtual void   Draw() override;
};
#endif

} // Core
} // CUBE
