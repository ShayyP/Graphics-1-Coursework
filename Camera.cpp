#include "Camera.h"

// Default constructor
Camera::Camera()
{
	_xRotation = 0.0f;
	_yRotation = 0.0f;
	_zRotation = 0.0f;
	_position = Vertex(0, 0, 0, 1);
}

// Constructor that takes rotation values and a pointer to a Vertex instance for the camera position
Camera::Camera(float xRotation, float yRotation, float zRotation, const Vertex& Position)
{
	_xRotation = xRotation;
	_yRotation = yRotation;
	_zRotation = zRotation;
	_position = Position;
}

// Destructor
Camera::~Camera()
{
}

// Accessor methods
float Camera::GetXRotation() 
{
	return _xRotation;
}

float Camera::GetYRotation()
{
	return _yRotation;
}

float Camera::GetZRotation()
{
	return _zRotation;
}

Vertex Camera::GetPosition() 
{
	return _position;
}