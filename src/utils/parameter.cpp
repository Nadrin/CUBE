/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/config.h>
#include <core/ui.h>

#include <utils/parameter.h>
#include <classes/shader.h>

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
	if(valuePtr) {
		Config->Unregister(this);
	}

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
	case Type::Quat:
		return TW_TYPE_QUAT4F;
	case Type::Dir3:
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
	else if(valueType == Type::Vec3 || valueType == Type::Color3 || valueType == Type::Dir3) {
		const vec3& v = GetRefConst<vec3>();
		s << v.x << "," << v.y << "," << v.z;
	}
	else if(valueType == Type::Vec4 || valueType == Type::Color4) {
		const vec4& v = GetRefConst<vec4>();
		s << v.x << "," << v.y << "," << v.z << "," << v.w;
	}
	else if(valueType == Type::Quat) {
		const quat& q = GetRefConst<quat>();
		s << q.x << "," << q.y << "," << q.z << "," << q.w;
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
	else if(valueType == Type::Vec3 || valueType == Type::Color3 || valueType == Type::Dir3) {
		vec3& v = GetRef<vec3>();
		s >> v.x; sep(s); s >> v.y; sep(s); s >> v.z;
	}
	else if(valueType == Type::Vec4 || valueType == Type::Color4) {
		vec4& v = GetRef<vec4>();
		s >> v.x; sep(s); s >> v.y; sep(s); s >> v.z; sep(s); s >> v.w;
	}
	else if(valueType == Type::Quat) {
		quat& q = GetRef<quat>();
		s >> q.x; sep(s); s >> q.y; sep(s); s >> q.z; sep(s); s >> q.w;
	}
	else
		return false;

	return true;
}

ShaderParameter::ShaderParameter(const Shader* shader, const std::string& name, Type type)
{
	switch(type) {
	case Type::Int:
		valuePtr = new int(); break;
	case Type::Float:
		valuePtr = new float(); break;
	case Type::Vec2:
		valuePtr = new vec2(); break;
	case Type::Vec3:
	case Type::Color3:
	case Type::Dir3:
		valuePtr = new vec3(); break;
	case Type::Vec4:
	case Type::Color4:
		valuePtr = new vec4(); break;
	default:
		assert(false);
	}

	valueType = type;

	Shader::Uniform* uniform = &shader->operator[](name);
	uniform->parameter = this;
	this->uniform = static_cast<void*>(uniform);

	context[0] = { this, CallbackContext::Component::X };
	context[1] = { this, CallbackContext::Component::Y };
	context[2] = { this, CallbackContext::Component::Z };
	context[3] = { this, CallbackContext::Component::W };

	ShaderParameter::Init("Shaders:" + shader->GetName() + "/" + name, "");
}

ShaderParameter::~ShaderParameter()
{
	Config->Unregister(this);
	delete valuePtr;
	valuePtr = nullptr;
}

void ShaderParameter::Init(const std::string& path, const std::string& def)
{
	valueIdent = Identifier(path);
	TwType uiType = GetUIType(valueType);

	if(Config->Register(this))
		Update();

	bar = UI->GetBar(valueIdent.category);
	if(!bar) {
		bar = UI->AddBar(valueIdent.category);
	}

	if(valueType == Type::Vec2 || valueType == Type::Vec3 || valueType == Type::Vec4) {
		UI->AddVariable(bar, Identifier(valueIdent.name+".X", valueIdent), uiType, GetCallback, SetCallback, &context[0], def);
		UI->AddVariable(bar, Identifier(valueIdent.name+".Y", valueIdent), uiType, GetCallback, SetCallback, &context[1], def);

		if(valueType == Type::Vec3 || valueType == Type::Vec4) {
			UI->AddVariable(bar, Identifier(valueIdent.name+".Z", valueIdent), uiType, GetCallback, SetCallback, &context[2], def);
		}
		if(valueType == Type::Vec4) {
			UI->AddVariable(bar, Identifier(valueIdent.name+".W", valueIdent), uiType, GetCallback, SetCallback, &context[3], def);
		}
	}
	else {
		UI->AddVariable(bar, valueIdent, uiType, GetCallback, SetCallback, &context[0], def);
	}
}

void ShaderParameter::Update()
{
	Shader::Uniform* uniform = static_cast<Shader::Uniform*>(this->uniform);

	switch(valueType) {
	case Type::Int:
		uniform->operator=(GetRefConst<int>()); break;
	case Type::Float:
		uniform->operator=(GetRefConst<float>()); break;
	case Type::Vec2:
		uniform->operator=(GetRefConst<vec2>()); break;
	case Type::Vec3:
	case Type::Color3:
	case Type::Dir3:
		uniform->operator=(GetRefConst<vec3>()); break;
	case Type::Vec4:
	case Type::Color4:
		uniform->operator=(GetRefConst<vec4>()); break;
	default:
		assert(false);
	}
}

void TW_CALL ShaderParameter::GetCallback(void* value, void* clientData)
{
	ShaderParameter::CallbackContext* ctx = static_cast<ShaderParameter::CallbackContext*>(clientData);
	const int component = static_cast<const int>(ctx->component);

	switch(ctx->sp->valueType) {
	case Type::Int:
		std::memcpy(value, ctx->sp->valuePtr, sizeof(int)); break;
	case Type::Float:
	case Type::Vec2:
	case Type::Vec3:
	case Type::Vec4:
		std::memcpy(value, static_cast<const float*>(ctx->sp->valuePtr) + component, sizeof(float)); break;
	case Type::Color3:
	case Type::Dir3:
		std::memcpy(value, ctx->sp->valuePtr, 3*sizeof(float)); break;
	case Type::Color4:
		std::memcpy(value, ctx->sp->valuePtr, 4*sizeof(float)); break;
	default:
		assert(false);
	}
}

void TW_CALL ShaderParameter::SetCallback(const void* value, void* clientData)
{
	ShaderParameter::CallbackContext* ctx = static_cast<ShaderParameter::CallbackContext*>(clientData);
	const int component = static_cast<const int>(ctx->component);

	switch(ctx->sp->valueType) {
	case Type::Int:
		std::memcpy(ctx->sp->valuePtr, value, sizeof(int)); break;
	case Type::Float:
	case Type::Vec2:
	case Type::Vec3:
	case Type::Vec4:
		std::memcpy(static_cast<float*>(ctx->sp->valuePtr) + component, value, sizeof(float)); break;
	case Type::Color3:
	case Type::Dir3:
		std::memcpy(ctx->sp->valuePtr, value, 3*sizeof(float)); break;
	case Type::Color4:
		std::memcpy(ctx->sp->valuePtr, value, 4*sizeof(float)); break;
	default:
		assert(false);
	}

	ctx->sp->Update();
}