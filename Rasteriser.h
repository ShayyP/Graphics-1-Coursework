#pragma once
#include "Framework.h"
#include "MD2Loader.h"
#include "Vertex.h"
#include "Matrix.h"
#include "Polygon3D.h"
#include "Model.h"
#include "Camera.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <vector>
#include <algorithm>
#include "Demo.h"
#include <string>

class Rasteriser : public Framework
{
public:
	bool Initialise();
	// Loads model and textures from md2 & pcx files
	bool LoadModel(const char* modelPath, const char* texturePath);
	// Matrix generators
	Matrix GenerateViewMatrix(Camera camera);
	Matrix GeneratePerspectiveMatrix(float d, float aspectRatio);
	Matrix GenerateScreenMatrix(float d, int width, int height);
	Matrix GenerateTranslationMatrix(float x, float y, float z);
	Matrix GenerateScalingMatrix(float scale);
	Matrix GenerateRotationMatrix(float x, float y, float z);
	// Draws text to the screen
	void DrawString(const Bitmap& bitmap, LPCTSTR text);
	// Updates model, called every frame
	void Update(const Bitmap& bitmap);
	// Drawing functions
	void DrawWireframe(const Bitmap& bitmap, Polygon3D poly);
	void DrawSolidFlat(const Bitmap& bitmap, Polygon3D poly);
	void MyDrawSolidFlat(const Bitmap& bitmap, Polygon3D poly);
	static int Signum(float x);
	static float Clamp(float value, float lower, float upper);
	void FillPolygonFlat(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF colour);
	void DrawGouraudBresenham(const Bitmap& bitmap, Polygon3D poly);
	void FillPolygonGouraud(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3);
	void DrawGouraudStandard(const Bitmap& bitmap, Polygon3D poly);
	void FillBottomGouraud(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3);
	void FillTopGouraud(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3);
	void DrawGouraudTextured(const Bitmap& bitmap, Polygon3D poly);
	void FillGouraudTextured(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3, UVPair uv1, UVPair uv2, UVPair uv3);
	void DrawTexturedCorrectedBresenham(const Bitmap& bitmap, Polygon3D poly);
	void FillTexturedCorrected(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3, UVPair uv1, UVPair uv2, UVPair uv3);
	void DrawTexturedCorrectedStandard(const Bitmap& bitmap, Polygon3D poly);
	void FillBottomTextured(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3, UVPair uv1, UVPair uv2, UVPair uv3);
	void FillTopTextured(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3, UVPair uv1, UVPair uv2, UVPair uv3);
	// Draws model using specified draw mode, called every frame
	void Render(const Bitmap& bitmap);
private:
	Demo _demo;
	Camera _camera;
	Model _model;
};

