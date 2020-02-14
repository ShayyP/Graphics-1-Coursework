#pragma once
#include "Vertex.h"
#include "AmbientLight.h"
#include <windows.h>

// Class for point light that loses intensity over range
// Inherits from ambient light, spot light inherits from this
class PointLight : public AmbientLight
{
public:
	// Constructors
	PointLight();
	PointLight(COLORREF colour, Vertex position, float a, float b, float c);
	PointLight(const PointLight& p);
	// Destructor
	~PointLight();
	// Accessors
	Vertex GetPosition() const;
	float GetA() const;
	float GetB() const;
	float GetC() const;
protected:
	Vertex _position;
	float _a;
	float _b;
	float _c;
};
