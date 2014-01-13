/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class Parameter;

namespace Core {

class Config
{
protected:
	struct Item {
		std::string value;
		Parameter* param;

		Item() : param(nullptr) {}
		Item(const std::string& value, Parameter* param=nullptr)
			: value(value), param(param) {}
	};

	std::string filename;
	std::map<std::string, Item> parameterMap;
protected:
	void Register(Parameter* param);
	virtual void Unregister(Parameter* param) {}
public:
	virtual ~Config() {}

	bool Read(const std::string& filename);
	virtual bool Write() { return false; }

	friend class Parameter;
};

#ifdef _DEBUG
class ConfigRW : public Config
{
protected:
	virtual void Unregister(Parameter* param);
public:
	virtual ~ConfigRW() { Write(); }
	virtual bool Write();
};
#endif

} // Core
} // CUBE