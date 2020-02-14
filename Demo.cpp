#include "Demo.h"

// Constructor
Demo::Demo() : _angles{ 0.0f }, _position{ 0.0f }
{
	// Initialising variables
	_frame = 0;
	_backface = false;
	_smoothShading = false;
	_specular = false;
	_scale = 1.0f;
	_stage = "Wireframe";
	_drawMode = "Wireframe";
	_movingAway = true;
	_shrinking = true;
	_model = "Models\\cube.md2";
	_texture = NULL;
	_changedModel = false;
	_ambientLight = NULL;
	_directionalLights = {};
	_pointLights = {};
	_spotLights = {};
}

// Destructor
Demo::~Demo()
{
}

// Accessors
bool Demo::GetBackface()
{
	return _backface;
}

bool Demo::GetSmoothShading()
{
	return _smoothShading;
}

bool Demo::GetSpecular()
{
	return _specular;
}

std::string Demo::GetStage()
{
	return _stage;
}

std::string Demo::GetDrawMode()
{
	return _drawMode;
}

AmbientLight Demo::GetAmbientLight()
{
	return _ambientLight;
}

std::vector<DirectionalLight> Demo::GetDirectionalLights()
{
	return _directionalLights;
}

std::vector<PointLight> Demo::GetPointLights()
{
	return _pointLights;
}

std::vector<SpotLight> Demo::GetSpotLights()
{
	return _spotLights;
}

float Demo::GetPosition(int index)
{
	return _position[index];
}

float Demo::GetScale()
{
	return _scale;
}

float Demo::GetRotation(int index)
{
	return DegreesToRadians(_angles[index]);
}

const char* Demo::GetModel()
{
	return _model;
}

const char* Demo::GetTexture()
{
	return _texture;
}

bool Demo::GetChangedModel()
{
	return _changedModel;
}

// Converts degrees to radians for use with trig functions
float Demo::DegreesToRadians(float degrees)
{
	return float(degrees * 3.14159265358979323846 / 180);
}

// Mutator
void Demo::SetChangedModel(bool changed)
{
	_changedModel = changed;
}

/*
Stages of demo:
1: Wireframe
2: Translation
3: Rotation
4: Scale
5: Backface culling
6: Solid fill ambient
7: Directional light
8: Point light
9: My Solid fill
10: Gouraud shading (Bresenham)
11: Specular lighting
12: Spot light
13: Textures
14: Textures with perspective correction -> loop back to start
*/

void Demo::Update()
{
	// Updates model depending on frame. Called in Rasteriser::Update
	switch (_frame)
	{
	case 100:
		_stage = "Translation";
		break;
	case 200:
		_stage = "Rotation";
		break;
	case 300:
		_stage = "Scaling";
		break;
	case 400:
		_stage = "Backface culling";
		_backface = true;
		break;
	case 500:
		_model = "Models\\marvin.md2";
		_changedModel = true;
		_stage = "Solid fill with ambient light";
		_drawMode = "Solid";
		_ambientLight = AmbientLight(RGB(0, 255, 255));
		break;
	case 600:
		_stage = "Solid fill with ambient and directional light";
		_directionalLights = { DirectionalLight(RGB(0, 255, 255), Vertex(1, 0, 0)) };
		break;
	case 725:
		_stage = "Solid fill with ambient, directional and point light";
		_pointLights = { PointLight(RGB(255, 255, 255), Vertex(50, 0, -50), 0, 1, 0) };
		break;
	case 875:
		_stage = "Solid fill using Bresenham algorithm";
		_drawMode = "MySolid";
		break;
	case 950:
		_stage = "Smooth shading using Bresenham algorithm";
		_drawMode = "Bresenham";
		_smoothShading = true;
		break;
	case 1050:
		_stage = "Specular lighting";
		_specular = true;
		break;
	case 1150:
		_stage = "Spot lighting";
		_directionalLights = {};
		_pointLights = {};
		_spotLights = { SpotLight(RGB(0,255,0), Vertex(0, 0, -50), 0, 1, 0, DegreesToRadians(15), DegreesToRadians(30)) };	
		break;
	case 1250:
		_model = "Models\\cube.md2";
		_texture = "Models\\lines.pcx";
		_changedModel = true;
		_drawMode = "Textured";
		_stage = "Smooth shading with textures (not corrected for perspective)";
		_directionalLights = { DirectionalLight(RGB(0, 255, 255), Vertex(1, 0, 0)) };
		_pointLights = _pointLights = { PointLight(RGB(255, 255, 255), Vertex(50, 0, -50), 0, 1, 0) };
		break;
	case 1350:
		_stage = "Textures corrected for perspective (not working correctly)";
		_drawMode = "TexturedCorrected";
		break;
	case 1450:
		// Resets back to wireframe
		_frame = 0;
		_backface = false;
		_smoothShading = false;
		_specular = false;
		_scale = 1.0f;
		_stage = "Wireframe";
		_drawMode = "Wireframe";
		_movingAway = true;
		_shrinking = true;
		_texture = NULL;
		_changedModel = true;
		_texture = NULL;
		_angles[0] = 0;
		_angles[1] = 0;
		_angles[2] = 0;
		_position[0] = 0;
		_position[1] = 0;
		_position[2] = 0;
		_ambientLight = NULL;
		_directionalLights = {};
		_pointLights = {};
		_spotLights = {};
	}

	// Makes model move away then back to original position
	if (_stage == "Translation")
	{
		if (_position[0] < 50 && _movingAway)
		{
			_position[0]++;
			_position[1]++;
			_position[2]++;
		}
		else if(_position[0] > 0 && !_movingAway)
		{
			_position[0]--;
			_position[1]--;
			_position[2]--;
		}
		else
		{
			_movingAway = false;
		}
	}

	// Makes model rotate 180 degrees in all 3 directions
	else if (_stage == "Rotation")
	{
		if (_angles[0] < 180)
		{
			_angles[0] += 2;
			_angles[1] += 2;
			_angles[2] += 2;
		}
	}

	// Makes model shrink and then return to original size
	else if (_stage == "Scaling")
	{
		_angles[0] = 0;
		_angles[1] = 0;
		_angles[2] = 0;
		if (_scale > 0.5 && _shrinking)
		{
			_scale -= 0.01f;
		}
		else if (_scale < 1 && !_shrinking)
		{
			_scale += 0.01f;
		}
		else
		{
			_shrinking = false;
		}
	}

	// Constantly rotates model slowly to show off model at all angles
	if (_frame >= 400)
	{
		_angles[1]++;
	}

	// Increments the counter
	_frame++;
}
