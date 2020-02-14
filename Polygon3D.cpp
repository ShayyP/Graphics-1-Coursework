#include "Polygon3D.h"

// Default constructor
Polygon3D::Polygon3D() : _indices{ 0 }, _uvIndices{ 0 }
{
	_averageZ = 0;
	_colour = RGB(0, 0, 0);
}

// Constructor that accepts 3 indices and uv indices
Polygon3D::Polygon3D(int index0, int index1, int index2, int uvIndex0, int uvIndex1, int uvIndex2)
{
	_indices[0] = index0;
	_indices[1] = index1;
	_indices[2] = index2;
	_uvIndices[0] = uvIndex0;
	_uvIndices[1] = uvIndex1;
	_uvIndices[2] = uvIndex2;
	_averageZ = 0;
	_colour = RGB(0, 0, 0);
}

// Copy constructor
Polygon3D::Polygon3D(const Polygon3D& p) 
{
	_indices[0] = p.GetIndex(0);
	_indices[1] = p.GetIndex(1);
	_indices[2] = p.GetIndex(2);
	_uvIndices[0] = p.GetUVIndex(0);
	_uvIndices[1] = p.GetUVIndex(1);
	_uvIndices[2] = p.GetUVIndex(2);
	_markedForCulling = p.GetCulling();
	_averageZ = p.GetAverageZ();
	_colour = p.GetColour();
}

// Destructor
Polygon3D::~Polygon3D()
{
}

// Accessors
int Polygon3D::GetIndex(int index) const
{
	return _indices[index];
}

int Polygon3D::GetUVIndex(int index) const
{
	return _uvIndices[index];
}

void Polygon3D::SetCulling(bool cull)
{
	_markedForCulling = cull;
}

bool Polygon3D::GetCulling() const
{
	return _markedForCulling;
}

void Polygon3D::SetAverageZ(float average)
{
	_averageZ = average;
}

float Polygon3D::GetAverageZ() const
{
	return _averageZ;
}

void Polygon3D::SetColour(COLORREF colour)
{
	_colour = colour;
}

COLORREF Polygon3D::GetColour() const
{
	return _colour;
}

Polygon3D& Polygon3D::operator=(const Polygon3D& rhs)
{
	// Only do the assignment if we are not assigning
	// to ourselves
	if (this != &rhs)
	{
		_indices[0] = rhs.GetIndex(0);
		_indices[1] = rhs.GetIndex(1);
		_indices[2] = rhs.GetIndex(2);
		_uvIndices[0] = rhs.GetUVIndex(0);
		_uvIndices[1] = rhs.GetUVIndex(1);
		_uvIndices[2] = rhs.GetUVIndex(2);
		_markedForCulling = rhs.GetCulling();
		_averageZ = rhs.GetAverageZ();
		_colour = rhs.GetColour();
	}
	return *this;
}

// Used by std::sort to order lists of this class
const bool Polygon3D::operator<(const Polygon3D& rhs) const
{
	return _averageZ < rhs.GetAverageZ();
}
