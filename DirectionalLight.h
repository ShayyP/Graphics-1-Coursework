#pragma once
#include "Vertex.h"
#include "AmbientLight.h"
#include <windows.h>

// Directional light inherits from Ambient light as they both share colour
class DirectionalLight : public AmbientLight
{
public:
	// Constructors
	DirectionalLight();
	DirectionalLight(COLORREF colour, Vertex direction);
	DirectionalLight(const DirectionalLight& p);
	// Destructor
	~DirectionalLight();
	// Accessor
	Vertex GetDirection() const;
private:
	Vertex _direction;
};

