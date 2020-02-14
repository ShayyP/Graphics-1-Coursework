#pragma once
#include "Vertex.h"

// Class that stores info for the camera which is used in the rendering pipeline and to calculate specular lighting
class Camera
{
public:
	// Constructors
	Camera();
	Camera(float xRotation, float yRotation, float zRotation, const Vertex& Position);
	// Destructors
	~Camera();

	// Accessors
	float GetXRotation();
	float GetYRotation();
	float GetZRotation();
	Vertex GetPosition();

private:
	float _xRotation;
	float _yRotation;
	float _zRotation;
	Vertex _position;
};

