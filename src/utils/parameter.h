/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Identifier
{
public:
	std::string name;
	std::string group;
	std::string category;

	static std::string DefaultCategory;

	Identifier();
	Identifier(const std::string& name);
	Identifier(const std::string& name, const Identifier& ident);
};

class Parameter
{
public:
	enum Type
	{
		Undefined = 0,
		Bool,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Double,
		String,
		Color3,
		Color4,
		Direction,
	};
	enum Flags
	{
		Default   = 0x00,
		Transient = 0x01,
		Hidden    = 0x02,
	};
protected:
	void* valuePtr;
	Type  valueType;
	Identifier valueIdent;

	TwBar* bar;

	static TwType GetUIType(Type type);

	void Init(const std::string& path, Flags flags);
	void Init(const std::string& path, const float step, Flags flags);
	void Init(const std::string& path, const float min, const float max, const float step, Flags flags);

	void Init(const std::string& path, const std::string& def, Flags flags);
public:
	template<typename T>
	Parameter(T& ref, Type type, const std::string& path, Flags flags)
		: valuePtr(&ref), valueType(type) { Init(path, flags); }

	template<typename T>
	Parameter(T& ref, Type type, const std::string& path, Flags flags, const std::string& def)
		: valuePtr(&ref), valueType(type) { Init(path, def, flags); }

	template<typename T>
	Parameter(T& ref, Type type, const float step, const std::string& path, Flags flags)
		: valuePtr(&ref), valueType(type)
	{ 
		Init(path, step, flags);
	}

	template<typename T>
	Parameter(T& ref, Type type, 
		const float min, const float max, const float step,
		const std::string& path, Flags flags)
		: valuePtr(&ref), valueType(type)
	{ 
		Init(path, min, max, step, flags);
	}

	virtual ~Parameter();

	Type GetType() const { return valueType; }
	const Identifier& GetIdent() const { return valueIdent; }
	template<typename T> T* GetPtr() const { return (T*)valuePtr; }
};

typedef Parameter Param;

} // CUBE