#pragma once
#include <iostream>
#include <vector>
#include "Vertex.h"
#include "Polygon3D.h"
#include "Matrix.h"
#include "Camera.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Texture.h"
#include "UVPair.h"

class Model
{
public:
	// Constructor
	Model();
	// Destructor
	~Model();
	// Accessors and mutators
	const std::vector<Polygon3D>& GetPolygons();
	const std::vector<Vertex>& GetVertices();
	const std::vector<Vertex>& GetTransformedVertices();
	const std::vector<UVPair>& GetUVPairs();
	size_t GetPolygonCount() const;
	size_t GetVertexCount() const;
	void AddVertex(float x, float y, float z);
	void AddPolygon(int i0, int i1, int i2, int uvIndex0, int uvIndex1, int uvIndex2);
	void AddTextureUV(float u, float v);
	Texture& GetTexture();
	// Other methods
	void ApplyTransformToLocalVertices(const Matrix& transform);
	void ApplyTransformToTransformedVertices(const Matrix& transform);
	void Dehomogenise();
	void CalculateBackfaces(Camera camera);
	void Sort(void);

	// Lighting calculation functions
	// Flat lighting
	void CalculateFlatLightingAmbient(AmbientLight ambientLight);
	void CalculateFlatLightingDirectional(std::vector<DirectionalLight> directionalLights);
	void CalculateFlatLightingPoint(std::vector<PointLight> pointLights);
	// Smooth lighting
	void CalculateSmoothLightingAmbient(AmbientLight ambientLight);
	void CalculateSmoothLightingDirectional(std::vector<DirectionalLight> directionalLights);
	void CalculateSmoothLightingPoint(std::vector<PointLight> pointLights);
	// Specular lighting
	void CalculateSmoothLightingDirectionalSpecular(std::vector<DirectionalLight> directionalLights, Camera camera);
	void CalculateSmoothLightingPointSpecular(std::vector<PointLight> pointLights, Camera camera);
	static float SmoothStep(float edge0, float edge1, float x);
	void CalculateSpotLighting(std::vector<SpotLight> spotLights, Camera camera);

	// Saves vertex normals
	void CalculateNormals();

private:
	// Collections
	std::vector<Polygon3D> _polygons;
	std::vector<Vertex> _vertices;
	std::vector<Vertex> _transformedVertices;
	std::vector<UVPair> _uvPairs;
	// Texture for model
	Texture _texture;
	// Reflection coefficients
	float _kAmbient = 0.2f;
	float _kDirectionalDiffuse = 0.5f;
	float _kDirectionalSpecular = 0.1f;
	float _kPointDiffuse = 0.4f;
	float _kPointSpecular = 0.4f;
	float _roughness = 0.5f;
};

