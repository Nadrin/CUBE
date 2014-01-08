/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#define render auto RenderFunction = [&](float Time)
#define commit Global::Run(RenderFunction)

namespace CUBE {

class Demo
{
public:
	virtual void Run() = 0;
};

} // CUBE