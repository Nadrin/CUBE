/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

#include <core/system.h>

namespace CUBE {

struct Dim
{
	unsigned int Width;
	unsigned int Height;
	unsigned int Depth;

	Dim()
		: Width(0), Height(0), Depth(0) {}
	Dim(const unsigned int w, const unsigned int h=1, const unsigned int d=1)
		: Width(w), Height(h), Depth(d) {}
};

struct ViewportDim : public Dim
{
	ViewportDim(const unsigned int divw=1, const unsigned int divh=1)
	{
		assert(divw != 0 && divh != 0);

		int dispWidth, dispHeight;
		Core::System::Instance()->GetDisplaySize(dispWidth, dispHeight);

		Width  = glm::min(dispWidth / divw, unsigned int(1));
		Height = glm::min(dispHeight / divh, unsigned int(1));
		Depth  = 1;
	}
};

} // CUBE