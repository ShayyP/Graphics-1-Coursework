#pragma once
#include "Vertex.h"
#include <windows.h>

// Class used to store info for an ambient light. All other lights inherit from this class
class AmbientLight
{
public:
	// Constructors
	AmbientLight();
	AmbientLight(COLORREF colour);
	AmbientLight(const AmbientLight& p);
	// Destructor
	~AmbientLight();
	// Accessors
	COLORREF GetColour() const;
private:
	COLORREF _colour;
};
