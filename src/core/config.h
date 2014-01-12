/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Parameter;

namespace Core {

class Config
{
private:
	struct Item {
		std::string Value;
		Parameter* Param;

		Item() : Param(nullptr) {}
		Item(const std::string& value) : Value(value), Param(nullptr) {}
	};

	std::string FileName;
	std::map<std::string, Item> ParameterMap;
public:
	Config() {}

	void Register(Parameter* param);
	void Unregister(Parameter* param);

	bool Load(const std::string& filename);
	bool Save();
};

} // Core
} // CUBE