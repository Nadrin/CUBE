/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

enum class PlacementMode {
	None,
	Vertical,
	Horizontal
};

namespace Core {

class UI
{
public:
	PlacementMode Placement;
	int Padding;
protected:
	bool Active;

	UI() : Active(false), Padding(10), Placement(PlacementMode::None) {}
public:
	virtual ~UI() {}

	bool IsActive() const { return Active;    }
	void Toggle()         { Active = !Active; }

	virtual TwBar* AddBar(const std::string& name) { return nullptr; }
	virtual void   AddVariable(TwBar* bar, const std::string& name, TwType type, void* data) {}
	virtual void   AddVariable(TwBar* bar, const std::string& name, TwType type, void* data, const std::string& def) {}
	virtual void   AddSeparator(TwBar* bar, const std::string& name) {}
	virtual void   AddSeparator(TwBar* bar, const std::string& name, const std::string& def) {}

	virtual void   Clear() {}
	virtual void   Arrange() {}
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
	int ClientWidth, ClientHeight;
protected:
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void UnicodeCharCallback(GLFWwindow* window, unsigned int character);
protected:
	void ParseName(const std::string& name, std::string& varname, std::string& vargrp) const;
	void PlaceBar(TwBar* bar, const int maxIndex);
public:
	TweakBarUI(GLFWwindow* window, const int width, const int height);
	virtual ~TweakBarUI();

	void TranslateKeyEvent(int key, int action);

	virtual TwBar* AddBar(const std::string& name) override;
	virtual void   AddVariable(TwBar* bar, const std::string& name, TwType type, void* data) override;
	virtual void   AddVariable(TwBar* bar, const std::string& name, TwType type, void* data, const std::string& def) override;
	virtual void   AddSeparator(TwBar* bar, const std::string& name) override;
	virtual void   AddSeparator(TwBar* bar, const std::string& name, const std::string& def) override;

	virtual void   Clear() override;
	virtual void   Arrange() override;
	virtual void   Draw() override;
};
#endif

} // Core
} // CUBE
