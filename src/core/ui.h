/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Identifier;

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
	int   Padding;
	float Precision;
protected:
	bool active;

	UI() : active(false), Padding(10), Precision(0.1f), Placement(PlacementMode::Horizontal) {}
public:
	virtual ~UI() {}

	bool IsActive() const { return active;    }
	void Toggle()         { active = !active; }

	virtual TwBar* AddBar(const std::string& name) { return nullptr; }
	virtual void   RemoveBar(TwBar* bar) {}
	virtual void   AddVariable(TwBar* bar, const Identifier& ident, TwType type, void* data) {}
	virtual void   AddVariable(TwBar* bar, const Identifier& ident, TwType type, void* data, const std::string& def) {}
	virtual void   AddVariable(TwBar* bar, const Identifier& ident, TwType type, 
		TwGetVarCallback getCallback, TwSetVarCallback setCallback, void* data, const std::string& def) {}

	virtual bool   RemoveVariable(TwBar* bar, const Identifier& ident) { return true; }
	virtual void   AddSeparator(TwBar* bar, const std::string& name) {}
	virtual void   AddSeparator(TwBar* bar, const std::string& name, const std::string& def) {}

	virtual void   RefreshBar(TwBar* bar) {}
	virtual TwBar* GetBar(const std::string& name) { return nullptr; }

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
	int clientWidth, clientHeight;
protected:
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void UnicodeCharCallback(GLFWwindow* window, unsigned int character);
protected:
	void PlaceBar(TwBar* bar, const int maxIndex);
	void SetDefaultDef(std::stringstream& buffer, TwType type, const std::string& def) const;
public:
	TweakBarUI(GLFWwindow* window, const int width, const int height);
	virtual ~TweakBarUI();

	void TranslateKeyEvent(int key, int action);

	virtual TwBar* AddBar(const std::string& name) override;
	virtual void   RemoveBar(TwBar* bar) override;
	virtual void   AddVariable(TwBar* bar, const Identifier& ident, TwType type, void* data) override;
	virtual void   AddVariable(TwBar* bar, const Identifier& ident, TwType type, void* data, const std::string& def) override;
	virtual void   AddVariable(TwBar* bar, const Identifier& ident, TwType type, 
		TwGetVarCallback getCallback, TwSetVarCallback setCallback, void* data, const std::string& def) override;

	virtual bool   RemoveVariable(TwBar* bar, const Identifier& ident) override;
	virtual void   AddSeparator(TwBar* bar, const std::string& name) override;
	virtual void   AddSeparator(TwBar* bar, const std::string& name, const std::string& def) override;

	virtual void   RefreshBar(TwBar* bar) override;
	virtual TwBar* GetBar(const std::string& name) override;

	virtual void   Clear() override;
	virtual void   Arrange() override;
	virtual void   Draw() override;
};
#endif

} // Core
} // CUBE
