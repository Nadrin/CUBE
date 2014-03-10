/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

// Implementation based on code by Guillaume Racicot.
template<class Owner, typename T, const T& (Owner::*Get)() const, void (Owner::*Set)(const T&)>
class Property
{
private:
	Owner* const owner;
public:
	Property(Owner* o) : owner(o)
	{}

	operator const T&() const
	{
		return (owner->*Get)();
	}

	void operator =(const T& value)
	{
		(owner->*Set)(value);
	}
};

} // CUBE