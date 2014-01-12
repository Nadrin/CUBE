/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
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

Parameter::~Parameter()
{
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

