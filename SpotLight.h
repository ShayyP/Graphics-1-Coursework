#pragma once
#include "Vertex.h"
#include <windows.h>
#include "PointLight.h"

// Spot light inherits from point light as they share many attributes
class SpotLight : public PointLight
{
public:
	SpotLight();
	SpotLight(COLORREF colour, Vertex position, float a, float b, float c, float innerAngle, float outerAngle);
	// Destructor
	~SpotLight();
	// Accessors
	float GetInnerAngle() const;
	float GetOuterAngle() const;
private:
	float _innerAngle;
	float _outerAngle;
};