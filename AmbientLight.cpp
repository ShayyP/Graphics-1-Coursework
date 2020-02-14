#include "AmbientLight.h"

// Constructors
AmbientLight::AmbientLight()
{
	_colour = RGB(0, 0, 0);
}

AmbientLight::AmbientLight(COLORREF colour)
{
	_colour = colour;
}

AmbientLight::AmbientLight(const AmbientLight& p)
{
	_colour = p.GetColour();
}

// Destructor
AmbientLight::~AmbientLight()
{
}

// Accessor
COLORREF AmbientLight::GetColour() const
{
	return _colour;
}
