#include "Vertex.h"

// Default constructor
Vertex::Vertex()
{
	_x = 0.0f;
	_y = 0.0f;
	_z = 0.0f;
	_w = 1.0f;
	_normalX = 0.0f;
	_normalY = 0.0f;
	_normalZ = 0.0f;
	_colour = RGB(0, 0, 0);
	_contributions = 0;
	_uvIndex = 0;
	_preTransformZ = 0;
}

// Constructor that accepts x and y values
Vertex::Vertex(float x, float y)
{
	_x = x;
	_y = y;
	_z = 0.0f;
	_w = 1.0f;
	_normalX = 0.0f;
	_normalY = 0.0f;
	_normalZ = 0.0f;
	_colour = RGB(0, 0, 0);
	_contributions = 0;
	_uvIndex = 0;
	_preTransformZ = 0;
}

// Constructor that accepts x, y and z values
Vertex::Vertex(float x, float y, float z)
{
	_x = x;
	_y = y;
	_z = z;
	_w = 1.0f;
	_normalX = 0.0f;
	_normalY = 0.0f;
	_normalZ = 0.0f;
	_colour = RGB(0, 0, 0);
	_contributions = 0;
	_uvIndex = 0;
	_preTransformZ = 0;
}

// Constructor that accepts x, y, z and w values
Vertex::Vertex(float x, float y, float z, float w)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
	_normalX = 0.0f;
	_normalY = 0.0f;
	_normalZ = 0.0f;
	_colour = RGB(0, 0, 0);
	_contributions = 0;
	_uvIndex = 0;
	_preTransformZ = 0;
}

// Copy constructor
Vertex::Vertex(const Vertex& other)
{
	_x = other.GetX();
	_y = other.GetY();
	_z = other.GetZ();
	_w = other.GetW();
	_normalX = other.GetNormal().GetX();
	_normalY = other.GetNormal().GetY();
	_normalZ = other.GetNormal().GetZ();
	_colour = other.GetColour();
	_contributions = other.GetContributions();
	_uvIndex = other.GetUVIndex();
	_preTransformZ = other.GetPreTransformZ();
}

// Destructor
Vertex::~Vertex()
{
}

// Accessors and mutators
float Vertex::GetX() const
{
	return _x;
}

void Vertex::SetX(const float x)
{
	_x = x;
}

float Vertex::GetY() const
{
	return _y;
}

void Vertex::SetY(const float y)
{
	_y = y;
}

float Vertex::GetZ() const
{
	return _z;
}

void Vertex::SetZ(const float z)
{
	_z = z;
}

float Vertex::GetW() const
{
	return _w;
}

void Vertex::SetW(const float w)
{
	_w = w;
}

Vertex Vertex::GetNormal() const
{
	return Vertex(_normalX, _normalY, _normalZ);
}

void Vertex::SetNormal(const Vertex normal)
{
	_normalX = normal.GetX();
	_normalY = normal.GetY();
	_normalZ = normal.GetZ();
}

COLORREF Vertex::GetColour() const
{
	return _colour;
}

void Vertex::SetColour(const COLORREF colour)
{
	_colour = colour;
}

int Vertex::GetContributions() const
{
	return _contributions;
}

void Vertex::IncreaseContributions()
{
	_contributions++;
}

void Vertex::SetContributions(const int cont)
{
	_contributions = cont;
}

int Vertex::GetUVIndex() const
{
	return _uvIndex;
}

void Vertex::SetUVIndex(const int index)
{
	_uvIndex = index;
}

float Vertex::GetPreTransformZ() const
{
	return _preTransformZ;
}

// Saves z value for use in texture perspective correction
void Vertex::SavePreTransformZ()
{
	_preTransformZ = _w;
}

// Dehomogenises the vertex by dividing all coordinates by w
void Vertex::Dehomogenise()
{
	SavePreTransformZ();
	_x /= _w;
	_y /= _w;
	_z /= _w;
	_w /= _w;
}

// Returns the normalised vector
Vertex Vertex::Normalise()
{
	float size = Length();
	return Vertex(_x / size, _y / size, _z / size, _w / size);
}

// Returns the length
float Vertex::Length()
{
	return float(sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2)));
}

// Operators
Vertex& Vertex::operator=(const Vertex& rhs)
{
	// Only do the assignment if we are not assigning
	// to ourselves
	if (this != &rhs)
	{
		_x = rhs.GetX();
		_y = rhs.GetY();
		_z = rhs.GetZ();
		_w = rhs.GetW();
		_normalX = rhs.GetNormal().GetX();
		_normalY = rhs.GetNormal().GetY();
		_normalZ = rhs.GetNormal().GetZ();
		_colour = rhs.GetColour();
		_contributions = rhs.GetContributions();
		_uvIndex = rhs.GetUVIndex();
		_preTransformZ = rhs.GetPreTransformZ();
	}
	return *this;
}

// The const at the end of the declaraion for '==" indicates that this operation does not change
// any of the member variables in this class.

bool Vertex::operator==(const Vertex& rhs) const
{
	return (_x == rhs.GetX() && _y == rhs.GetY() && _z == rhs.GetZ() && _w == rhs.GetW());
}

// You can see three different uses of 'const' here:
//
// The first const indicates that the method changes the return value, but it is not moved in memory
// The second const indicates that the parameter is passed by reference, but it is not modified
// The third const indicates that the operator does not change any of the memory variables in the class.

const Vertex Vertex::operator+(const Vertex& rhs) const
{
	return Vertex(_x + rhs.GetX(), _y + rhs.GetY(), _z + rhs.GetZ(), _w + rhs.GetW());
}

const Vertex Vertex::operator-(const Vertex& rhs) const
{
	return Vertex(_x - rhs.GetX(), _y - rhs.GetY(), _z - rhs.GetZ(), _w - rhs.GetW());
}

//Cross product
const Vertex Vertex::operator*(const Vertex& rhs) const
{
	float x = _y * rhs.GetZ() - _z * rhs.GetY();
	float y = _z * rhs.GetX() - _x * rhs.GetZ();
	float z = _x * rhs.GetY() - _y * rhs.GetX();
	return Vertex(x, y, z, 1);
}

//Dot product
const float Vertex::operator&(const Vertex& rhs) const
{
	return (_x * rhs.GetX() + _y * rhs.GetY() + _z * rhs.GetZ());
}
