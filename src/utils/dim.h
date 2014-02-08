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
	Dim(const Dim& other)
		: Width(other.Width), Height(other.Height), Depth(other.Depth) {}

	virtual unsigned int GetWidth() const  { return Width;  }
	virtual unsigned int GetHeight() const { return Height; }
	virtual unsigned int GetDepth() const  { return Depth;  }

	virtual Dim* Clone() const
	{
		return new Dim(*this);
	}
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
	ViewportDim(const ViewportDim& other) : Dim(other) {}

	virtual Dim* Clone() const override
	{
		return new ViewportDim(*this);
	}
};

struct FractDim : public Dim
{
	unsigned int DivWidth;
	unsigned int DivHeight;
	unsigned int DivDepth;

	FractDim()
		: Dim(), DivWidth(1), DivHeight(1), DivDepth(1) {}
	FractDim(const unsigned int w, const unsigned int h=1, const unsigned int d=1)
		: Dim(), DivWidth(w), DivHeight(h), DivDepth(d)
	{
		assert(w != 0 && h != 0 && d != 0);
	}
	FractDim(const FractDim& other)
		: Dim(other), 
		  DivWidth(other.DivWidth), DivHeight(other.DivHeight), DivDepth(other.DivDepth)
	{}

	void FromBaseDim(const Dim& base)
	{
		Width  = base.GetWidth();
		Height = base.GetHeight();
		Depth  = base.GetDepth();
	}

	virtual unsigned int GetWidth() const override  { return Width / DivWidth;   }
	virtual unsigned int GetHeight() const override { return Height / DivHeight; }
	virtual unsigned int GetDepth() const override  { return Depth / DivDepth;   }

	virtual Dim* Clone() const override
	{
		return new FractDim(*this);
	}
};

} // CUBE