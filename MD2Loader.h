#pragma once
#include "Model.h"

// Declare typedefs used by the MD2Loader to call the methods to add a vertex, 
// add a polygon and add a texture UV to the lists

typedef void (Model::*AddVertex)(float x, float y, float z);
typedef void (Model::*AddPolygon)(int i0, int i1, int i2, int uvIndex0, int uvIndex1, int uvIndex2);
typedef void (Model::*AddTextureUV)(float u, float v);

class MD2Loader
{
	public:
		MD2Loader();
		~MD2Loader();
		static bool LoadModel(const char* md2Filename, const char * textureFilename, Model& model, AddPolygon addPolygon, AddVertex addVertex, AddTextureUV addTextureUV);
};
