#include "DirectionalLight.h"

// Constructors
DirectionalLight::DirectionalLight() : AmbientLight()
{
	_direction = Vertex();
}

DirectionalLight::DirectionalLight(COLORREF colour, Vertex position) : AmbientLight(colour)
{
	_direction = position;
}

DirectionalLight::DirectionalLight(const DirectionalLight& p) : AmbientLight(p.GetColour())
{
	_direction = p.GetDirection();
}

// Destructor
DirectionalLight::~DirectionalLight()
{
}

// Accessor
Vertex DirectionalLight::GetDirection() const
{
	return _direction;
}
