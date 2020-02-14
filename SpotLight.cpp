#include "SpotLight.h"

// Default constructor
SpotLight::SpotLight() : PointLight()
{
	_innerAngle = 0.0f;
	_outerAngle = 0.0f;
}

// Other constructor
SpotLight::SpotLight(COLORREF colour, Vertex position, float a, float b, float c, float innerAngle, float outerAngle) : PointLight(colour, position, a, b, c)
{
	_innerAngle = innerAngle;
	_outerAngle = outerAngle;
}

// Destructor
SpotLight::~SpotLight()
{
}

// Accessors
float SpotLight::GetInnerAngle() const
{
	return _innerAngle;
}

float SpotLight::GetOuterAngle() const
{
	return _outerAngle;
}

