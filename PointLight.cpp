#include "PointLight.h"

// Constructors
PointLight::PointLight() : AmbientLight()
{
	_position = Vertex();
	_a = 0.0f;
	_b = 0.0f;
	_c = 0.0f;
}

PointLight::PointLight(COLORREF colour, Vertex position, float a, float b, float c) : AmbientLight(colour)
{
	_position = position;
	_a = a;
	_b = b;
	_c = c;
}

PointLight::PointLight(const PointLight& p) : AmbientLight(p.GetColour())
{
	_position = p.GetPosition();
	_a = p.GetA();
	_b = p.GetB();
	_c = p.GetC();
}

// Destructor
PointLight::~PointLight()
{
}

// Accessors
Vertex PointLight::GetPosition() const
{
	return _position;
}

float PointLight::GetA() const
{
	return _a;
}

float PointLight::GetB() const
{
	return _b;
}

float PointLight::GetC() const
{
	return _c;
}
