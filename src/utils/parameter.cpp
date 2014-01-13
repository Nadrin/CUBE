/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/config.h>
#include <core/ui.h>
#include <utils/parameter.h>

using namespace CUBE;

std::string Identifier::DefaultCategory("Parameters");

Identifier::Identifier()
{
	category = DefaultCategory;
}

Identifier::Identifier(const std::string& name, const Identifier& ident)
	: name(name), group(ident.group), category(ident.category)
{
}

Identifier::Identifier(const std::string& name)
{
	size_t categorySep = name.find_first_of(':', 0);
	if(categorySep == std::string::npos) {
		categorySep    = 0;
		this->category = DefaultCategory;
	}
	else {
		this->category = name.substr(0, categorySep);
		categorySep++;
	}

	size_t groupSep = name.find_first_of('/', categorySep);
	if(groupSep == std::string::npos) {
		this->name = name;
	}
	else {
		this->name  = name.substr(groupSep+1);
		this->group = name.substr(categorySep, groupSep-categorySep);
	}
}

std::string Identifier::ToString() const
{
	std::stringstream s;
	if(!category.empty())
		s << category << ":";
	if(!group.empty())
		s << group << "/";
	s << name;

	return s.str();
}

Parameter::~Parameter()
{
	Config->Unregister(this);

	if(bar) {
		if(valueType == Type::Vec2 || valueType == Type::Vec3 || valueType == Type::Vec4) {
			UI->RemoveVariable(bar, Identifier(valueIdent.name+".X", valueIdent));
			UI->RemoveVariable(bar, Identifier(valueIdent.name+".Y", valueIdent));
			if(valueType == Type::Vec3 || valueType == Type::Vec4) {
				UI->RemoveVariable(bar, Identifier(valueIdent.name+".Z", valueIdent));
			}
			if(valueType == Type::Vec4) {
				UI->RemoveVariable(bar, Identifier(valueIdent.name+".W", valueIdent));
			}
		}
		else {
			UI->RemoveVariable(bar, valueIdent);
		}
	}
}

void Parameter::Init(const std::string& path, Flags flags)
{
	Init(path, "", flags);
}

void Parameter::Init(const std::string& path, const float step, Flags flags)
{
	std::stringstream def;
	def << "step=" << step;
	Init(path, def.str(), flags);
}

void Parameter::Init(const std::string& path, const float min, const float max, const float step, Flags flags)
{
	std::stringstream def;
	def << "min=" << min << " max=" << max << " step=" << step;
	Init(path, def.str(), flags);
}

void Parameter::Init(const std::string& path, const std::string& def, Flags flags)
{
	valueIdent = Identifier(path);
	TwType uiType = GetUIType(valueType);

	if(!(flags & Flags::Transient))
		Config->Register(this);

	if(flags & Flags::Hidden || uiType == TW_TYPE_UNDEF) {
		bar = nullptr;
	}
	else {
		bar = UI->GetBar(valueIdent.category);
		if(!bar) {
			bar = UI->AddBar(valueIdent.category);
		}

		if(valueType == Type::Vec2 || valueType == Type::Vec3 || valueType == Type::Vec4) {
			float* value = (float*)valuePtr;
			UI->AddVariable(bar, Identifier(valueIdent.name+".X", valueIdent), uiType, &value[0], def);
			UI->AddVariable(bar, Identifier(valueIdent.name+".Y", valueIdent), uiType, &value[1], def);

			if(valueType == Type::Vec3 || valueType == Type::Vec4) {
				UI->AddVariable(bar, Identifier(valueIdent.name+".Z", valueIdent), uiType, &value[2], def);
			}
			if(valueType == Type::Vec4) {
				UI->AddVariable(bar, Identifier(valueIdent.name+".W", valueIdent), uiType, &value[2], def);
			}
		}
		else {
			UI->AddVariable(bar, valueIdent, uiType, valuePtr, def);
		}
	}
}

TwType Parameter::GetUIType(Type type)
{
	switch(type) {
	case Type::Bool:
		return TW_TYPE_BOOLCPP;
	case Type::Int:
		return TW_TYPE_INT32;
	case Type::Float:
	case Type::Vec2:
	case Type::Vec3:
	case Type::Vec4:
		return TW_TYPE_FLOAT;
	case Type::Double:
		return TW_TYPE_DOUBLE;
	case Type::String:
		return TW_TYPE_STDSTRING;
	case Type::Color3:
		return TW_TYPE_COLOR3F;
	case Type::Color4:
		return TW_TYPE_COLOR4F;
	case Type::Direction:
		return TW_TYPE_DIR3F;
	default:
		return TW_TYPE_UNDEF;
	}
}

std::string Parameter::ToString() const
{
	std::ostringstream s;

	if(valueType == Type::Bool)
		s << GetRefConst<bool>()?"1":"0";
	else if(valueType == Type::Int)
		s << GetRefConst<int>();
	else if(valueType == Type::Float)
		s << GetRefConst<float>();
	else if(valueType == Type::Double)
		s << GetRefConst<double>();
	else if(valueType == Type::String)
		s << GetRefConst<std::string>();
	else if(valueType == Type::Vec2) {
		const vec2& v = GetRefConst<vec2>();
		s << v.x << "," << v.y;
	}
	else if(valueType == Type::Vec3 || valueType == Type::Color3 || valueType == Type::Direction) {
		const vec3& v = GetRefConst<vec3>();
		s << v.x << "," << v.y << "," << v.z;
	}
	else if(valueType == Type::Vec4 || valueType == Type::Color4) {
		const vec4& v = GetRefConst<vec4>();
		s << v.x << "," << v.y << "," << v.z << "," << v.w;
	}
	else
		throw std::runtime_error("Invalid parameter type!");

	return s.str();
}

bool Parameter::FromString(const std::string& data)
{
	std::istringstream s(data);
	auto sep = [](std::istringstream& s) { if(s.peek() == ',') s.ignore(); };
	
	if(valueType == Type::Bool)
		GetRef<bool>() = (data == "1");
	else if(valueType == Type::Int)
		s >> GetRef<int>();
	else if(valueType == Type::Float)
		s >> GetRef<float>();
	else if(valueType == Type::Double)
		s >> GetRef<double>();
	else if(valueType == Type::String)
		s >> GetRef<std::string>();
	else if(valueType == Type::Vec2) {
		vec2& v = GetRef<vec2>();
		s >> v.x; sep(s); s >> v.y;
	}
	else if(valueType == Type::Vec3 || valueType == Type::Color3 || valueType == Type::Direction) {
		vec3& v = GetRef<vec3>();
		s >> v.x; sep(s); s >> v.y; sep(s); s >> v.z;
	}
	else if(valueType == Type::Vec4 || valueType == Type::Color4) {
		vec4& v = GetRef<vec4>();
		s >> v.x; sep(s); s >> v.y; sep(s); s >> v.z; sep(s); s >> v.w;
	}
	else
		return false;

	return true;
}
