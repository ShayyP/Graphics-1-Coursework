#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <vector>
#include <string>

/* Class used to demonstrate my program for marking. Essentially just runs on a counter, updating the model at set intervals
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

#pragma once
class Demo
{
public:
	// Constructor
	Demo();
	// Destructor
	~Demo();
	// Accessors
	bool GetBackface();
	bool GetSmoothShading();
	bool GetSpecular();
	std::string GetStage();
	std::string GetDrawMode();
	AmbientLight GetAmbientLight();
	std::vector<DirectionalLight> GetDirectionalLights();
	std::vector<PointLight> GetPointLights();
	std::vector<SpotLight> GetSpotLights();
	float GetPosition(int index);
	float GetScale();
	float GetRotation(int index);
	const char* GetModel();
	const char* GetTexture();
	bool GetChangedModel();
	// Useful function
	static float DegreesToRadians(float degrees);
	// Mutator
	void SetChangedModel(bool changed);
	// Updates the program
	void Update();
private:
	// Counter
	int _frame;
	// Specify whether backface culling, smooth shading and specular lighting are to be used respectively
	bool _backface;
	bool _smoothShading;
	bool _specular;
	// Saves rotation, position and scale of model
	float _angles[3];
	float _position[3];
	float _scale;
	// Stage is string displayed to screen, draw mode specifies which function is to be used to draw the model
	std::string _stage;
	std::string _drawMode;
	// Lights to be applied
	AmbientLight _ambientLight;
	std::vector<DirectionalLight> _directionalLights;
	std::vector<PointLight> _pointLights;
	std::vector<SpotLight> _spotLights;
	// Used to make model move away then back again and shrink then grow back to full size
	bool _movingAway;
	bool _shrinking;
	// Path of model and texture
	const char* _model;
	const char* _texture;
	// Specifies if model has been changed and needs to be reloaded from file
	bool _changedModel;
};

