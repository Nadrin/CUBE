/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <core/config.h>
#include <utils/parameter.h>

using namespace CUBE;
using namespace CUBE::Core;

bool Config::Register(Parameter* param)
{
	std::string name = param->GetIdent().ToString();

	auto it = parameterMap.find(name);
	if(it != parameterMap.end()) {
		it->second.param = param;
		param->FromString(it->second.value);
		return true;
	}
	else {
		parameterMap[name] = Item(param->ToString(), param);
		return false;
	}
}

bool Config::Read(const std::string& filename)
{
	this->filename = filename;

	std::ifstream file(filename.c_str());
	if(!file.is_open()) {
		System::Instance()->Log("Warning: Could not open config file: %s\n", filename.c_str());
		return false;
	}

	parameterMap.clear();
	while(!file.eof()) {
		std::string line, ident, value;
		std::getline(file, line);

		std::stringstream sline(line);
		if(!std::getline(sline, ident, '=') || ident.empty())
			continue;
		if(!std::getline(sline, value, '=') || value.empty())
			continue;

		parameterMap[ident] = Item(value);
	}

	System::Instance()->Log("Read %d parameters from config file: %s\n", parameterMap.size(), filename.c_str());
	return true;
}

#ifdef _DEBUG
bool ConfigRW::Write()
{
	if(filename.empty())
		return false;

	std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
	if(!file.is_open()) {
		System::Instance()->Log("Warning: Could not save config file: %s\n", filename.c_str());
		return false;
	}

	for(auto& kv : parameterMap) {
		if(kv.second.param) {
			kv.second.value = kv.second.param->ToString();
		}
		file << kv.first << "=" << kv.second.value << std::endl;
	}
	System::Instance()->Log("Configuration saved.\n");
	return true;
}

void ConfigRW::Unregister(Parameter* param)
{
	auto it = parameterMap.find(param->GetIdent().ToString());
	if(it != parameterMap.end()) {
		it->second.param = nullptr;
		it->second.value = param->ToString();
	}
}
#endif
