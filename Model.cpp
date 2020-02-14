#include "Model.h"
#include <algorithm>
#include <functional>
#include <math.h>

// Default constructor
Model::Model() 
{
}

// Destructor
Model::~Model() 
{
}

// Accessor methods
const std::vector<Polygon3D>& Model::GetPolygons()
{
	return _polygons;
}

const std::vector<Vertex>& Model::GetVertices()
{
	return _vertices;
}

const std::vector<Vertex>& Model::GetTransformedVertices()
{
	return _transformedVertices;
}

const std::vector<UVPair>& Model::GetUVPairs()
{
	return _uvPairs;
}

size_t Model::GetPolygonCount() const
{
	return _polygons.size();
}

size_t Model::GetVertexCount() const 
{
	return _vertices.size();
}

// Adds new vertex to the _vertices vector
void Model::AddVertex(float x, float y, float z) 
{
	_vertices.push_back(Vertex(x, y, z, 1));
}

// Adds new polygon to the _polygons vector
void Model::AddPolygon(int i0, int i1, int i2, int uvIndex0, int uvIndex1, int uvIndex2)
{
	_polygons.push_back(Polygon3D(i0, i1, i2, uvIndex0, uvIndex1, uvIndex2));
}

// Adds new UV pair to the _uvPairs vector
void Model::AddTextureUV(float u, float v)
{
	_uvPairs.push_back(UVPair(u, v));
}

// Returns model texture
Texture& Model::GetTexture()
{
	return _texture;
}

// Applies tranformation to local vertices then adds the result to _transformedVertices which is cleared each time this method is called
void Model::ApplyTransformToLocalVertices(const Matrix& transform)
{
	_transformedVertices.clear();
	for(Vertex vertex : _vertices) 
	{
		_transformedVertices.push_back(transform * vertex);
	}
}

// Applies tranformation to tranformed vertices then overwrites the existing value with the new result
void Model::ApplyTransformToTransformedVertices(const Matrix& transform)
{
	for (Vertex& vertex : _transformedVertices) 
	{
		vertex = transform * vertex;
	}
}

// Dehomogenises all transformed vertices
void Model::Dehomogenise()
{
	for (Vertex& vertex : _transformedVertices)
	{
		vertex.Dehomogenise();
	}
}

// Calculates whether each polygon should be marked for culling or not
void Model::CalculateBackfaces(Camera camera)
{
	// Loops through all polygons in the model
	for (Polygon3D &poly : _polygons) 
	{
		// Ensures that polygon is not marked for culling before calculations are made incase it has now moved into view
		poly.SetCulling(false);
		// Gets vertices in polygon
		Vertex vertex0 = GetTransformedVertices()[poly.GetIndex(0)];
		Vertex vertex1 = GetTransformedVertices()[poly.GetIndex(1)];
		Vertex vertex2 = GetTransformedVertices()[poly.GetIndex(2)];

		// Gets difference between vertex0 and vertex1
		Vertex vectorA = vertex0 - vertex1;
		// Gets difference between vertex0 and vertex2
		Vertex vectorB = vertex0 - vertex2;

		// Calculates the normal vector
		Vertex normalVector = vectorB * vectorA;

		// Calculates eye vector using camera position
		Vertex eyeVector = vertex0 - camera.GetPosition();

		// If the dot product of the normal and eye vector is less than 0 then the polygon is marked for culling as it is not facing forwards
		if ((normalVector & eyeVector) < 0)
		{
			poly.SetCulling(true);
		}
	}
}

// Sorts polygons in the model so in descending order of average z values
void Model::Sort(void)
{
	// Loops through all polygons in the model
	for (Polygon3D &poly : _polygons)
	{
		// Gets vertices in polygon
		Vertex vertex0 = GetTransformedVertices()[poly.GetIndex(0)];
		Vertex vertex1 = GetTransformedVertices()[poly.GetIndex(1)];
		Vertex vertex2 = GetTransformedVertices()[poly.GetIndex(2)];

		// Calculates avergae z value for the 3 vertices and stores it in the polygon instance
		poly.SetAverageZ((vertex0.GetZ() + vertex1.GetZ() + vertex2.GetZ()) / 3);
	}
	// Uses std::sort to sort the list of polygons in descending order of average z values
	std::sort(_polygons.begin(), _polygons.end(), std::less<Polygon3D>());
	std::reverse(_polygons.begin(), _polygons.end());
}

// Applies ambient lighting to each polygon in the model
void Model::CalculateFlatLightingAmbient(AmbientLight ambientLight)
{
	float rgb[3];
	for (Polygon3D& poly : _polygons)
	{
		rgb[0] = GetRValue(ambientLight.GetColour());
		rgb[1] = GetGValue(ambientLight.GetColour());
		rgb[2] = GetBValue(ambientLight.GetColour());

		rgb[0] *= _kAmbient;
		rgb[1] *= _kAmbient;
		rgb[2] *= _kAmbient;

		poly.SetColour(RGB(rgb[0], rgb[1], rgb[2]));
	}
}

// Applies directional lighting to each polygon in the model
void Model::CalculateFlatLightingDirectional(std::vector<DirectionalLight> directionalLights)
{
	float rgbTotal[3];
	float rgbTemp[3];

	// Loops through all polygons in the model
	for (Polygon3D& poly : _polygons)
	{
		//Resets total rgb to ambient light
		rgbTotal[0] = GetRValue(poly.GetColour());
		rgbTotal[1] = GetGValue(poly.GetColour());
		rgbTotal[2] = GetBValue(poly.GetColour());

		// Gets vertices in polygon
		Vertex vertex0 = GetTransformedVertices()[poly.GetIndex(0)];
		Vertex vertex1 = GetTransformedVertices()[poly.GetIndex(1)];
		Vertex vertex2 = GetTransformedVertices()[poly.GetIndex(2)];

		// Gets difference between vertex0 and vertex1
		Vertex vectorA = vertex0 - vertex1;
		// Gets difference between vertex0 and vertex2
		Vertex vectorB = vertex0 - vertex2;

		// Calculates the normal vector
		Vertex normalVector = vectorB * vectorA;

		// Loops through all directional light sources
		for (DirectionalLight light : directionalLights)
		{
			// Sets temp rgb values to light rgb intensity
			rgbTemp[0] = GetRValue(light.GetColour());
			rgbTemp[1] = GetGValue(light.GetColour());
			rgbTemp[2] = GetBValue(light.GetColour());

			// Modulates temp rgb by material reflectance coefficient 
			rgbTemp[0] *= _kDirectionalDiffuse;
			rgbTemp[1] *= _kDirectionalDiffuse;
			rgbTemp[2] *= _kDirectionalDiffuse;

			// Gets dot product of polygon normal vector and light source direction
			float dotProduct = light.GetDirection().Normalise() & normalVector.Normalise();
			if (dotProduct < 0)
			{
				dotProduct = 0;
			}

			// Multiplies rgb values by dot product
			rgbTemp[0] *= dotProduct;
			rgbTemp[1] *= dotProduct;
			rgbTemp[2] *= dotProduct;

			// Add temp rgb to total rgb
			rgbTotal[0] += rgbTemp[0];
			rgbTotal[1] += rgbTemp[1];
			rgbTotal[2] += rgbTemp[2];
		}

		// Clamps rgb values between 0 and 255
		for (float &value : rgbTotal)
		{
			value = value <= 0 ? 0 : value <= 255 ? value : 255;
		}

		// Stored colour in polygon
		poly.SetColour(RGB(rgbTotal[0], rgbTotal[1], rgbTotal[2]));
	}
}

// Applies point lighting to each polygon in the model
void Model::CalculateFlatLightingPoint(std::vector<PointLight> pointLights)
{
	float rgbTotal[3];
	float rgbTemp[3];

	// Loops through all polygons in the model
	for (Polygon3D& poly : _polygons)
	{
		//Resets total rgb to current light
		rgbTotal[0] = GetRValue(poly.GetColour());
		rgbTotal[1] = GetGValue(poly.GetColour());
		rgbTotal[2] = GetBValue(poly.GetColour());

		// Gets vertices in polygon
		Vertex vertex0 = GetTransformedVertices()[poly.GetIndex(0)];
		Vertex vertex1 = GetTransformedVertices()[poly.GetIndex(1)];
		Vertex vertex2 = GetTransformedVertices()[poly.GetIndex(2)];

		// Gets difference between vertex0 and vertex1
		Vertex vectorA = vertex0 - vertex1;
		// Gets difference between vertex0 and vertex2
		Vertex vectorB = vertex0 - vertex2;

		// Calculates the normal vector
		Vertex normalVector = vectorB * vectorA;

		// Loops through all point light sources
		for (PointLight light : pointLights)
		{
			// Sets temp rgb values to light rgb intensity
			rgbTemp[0] = GetRValue(light.GetColour());
			rgbTemp[1] = GetGValue(light.GetColour());
			rgbTemp[2] = GetBValue(light.GetColour());

			// Modulates temp rgb by material reflectance coefficient 
			rgbTemp[0] *= _kPointDiffuse;
			rgbTemp[1] *= _kPointDiffuse;
			rgbTemp[2] *= _kPointDiffuse;

			// Gets attentuation value and applies it to rgb
			Vertex difference = vertex0 - light.GetPosition();
			Vertex normalisedDifference = difference.Normalise();
			float d = difference.Length();
			float atten = 1 / (light.GetA() + light.GetB() * d + light.GetC() * pow(d, 2));
			atten *= 100;

			rgbTemp[0] *= atten;
			rgbTemp[1] *= atten;
			rgbTemp[2] *= atten;

			// Gets dot product of of polygon and light sources normal vectors
			float dotProduct = normalisedDifference & normalVector.Normalise();
			if (dotProduct < 0)
			{
				dotProduct = 0;
			}

			// Multiplies rgb values by dot product
			rgbTemp[0] *= dotProduct;
			rgbTemp[1] *= dotProduct;
			rgbTemp[2] *= dotProduct;

			// Add temp rgb to total rgb
			rgbTotal[0] += rgbTemp[0];
			rgbTotal[1] += rgbTemp[1];
			rgbTotal[2] += rgbTemp[2];
		}

		// Clamps rgb values between 0 and 255
		for (float &value : rgbTotal)
		{
			value = value <= 0 ? 0 : value <= 255 ? value : 255;
		}

		// Stored colour in polygon
		poly.SetColour(RGB(rgbTotal[0], rgbTotal[1], rgbTotal[2]));
	}
}

// Applies ambient lighting to each vertex in the model
void Model::CalculateSmoothLightingAmbient(AmbientLight ambientLight)
{
	float rgb[3];

	// Loops through all vertices
	for (Vertex& vertex : _transformedVertices)
	{
		rgb[0] = GetRValue(ambientLight.GetColour());
		rgb[1] = GetGValue(ambientLight.GetColour());
		rgb[2] = GetBValue(ambientLight.GetColour());

		rgb[0] *= _kAmbient;
		rgb[1] *= _kAmbient;
		rgb[2] *= _kAmbient;

		vertex.SetColour(RGB(rgb[0], rgb[1], rgb[2]));
	}
}

// Applies directional lighting to each vertex in the model
void Model::CalculateSmoothLightingDirectional(std::vector<DirectionalLight> directionalLights)
{
	float rgbTotal[3];
	float rgbTemp[3];

	// Loops through all vertices
	for (Vertex& vertex : _transformedVertices)
	{
		//Resets total rgb to ambient light
		rgbTotal[0] = GetRValue(vertex.GetColour());
		rgbTotal[1] = GetGValue(vertex.GetColour());
		rgbTotal[2] = GetBValue(vertex.GetColour());

		// Loops through all directional light sources
		for (DirectionalLight light : directionalLights)
		{
			// Sets temp rgb values to light rgb intensity
			rgbTemp[0] = GetRValue(light.GetColour());
			rgbTemp[1] = GetGValue(light.GetColour());
			rgbTemp[2] = GetBValue(light.GetColour());

			// Modulates temp rgb by material reflectance coefficient 
			rgbTemp[0] *= _kDirectionalDiffuse;
			rgbTemp[1] *= _kDirectionalDiffuse;
			rgbTemp[2] *= _kDirectionalDiffuse;

			// Gets dot product of polygon normal vector and light source direction
			float dotProduct = light.GetDirection().Normalise() & vertex.GetNormal().Normalise();
			if (dotProduct < 0)
			{
				dotProduct = 0;
			}

			// Multiplies rgb values by dot product
			rgbTemp[0] *= dotProduct;
			rgbTemp[1] *= dotProduct;
			rgbTemp[2] *= dotProduct;

			// Add temp rgb to total rgb
			rgbTotal[0] += rgbTemp[0];
			rgbTotal[1] += rgbTemp[1];
			rgbTotal[2] += rgbTemp[2];
		}

		// Clamps rgb values between 0 and 255
		for (float& value : rgbTotal)
		{
			value = value <= 0 ? 0 : value <= 255 ? value : 255;
		}

		// Stored colour in vertex
		vertex.SetColour(RGB(rgbTotal[0], rgbTotal[1], rgbTotal[2]));
	}
}

// Applies point lighting to each vertex in the model
void Model::CalculateSmoothLightingPoint(std::vector<PointLight> pointLights)
{
	float rgbTotal[3];
	float rgbTemp[3];

	// Loops through all vertices
	for (Vertex& vertex : _transformedVertices)
	{
		//Resets total rgb to current light
		rgbTotal[0] = GetRValue(vertex.GetColour());
		rgbTotal[1] = GetGValue(vertex.GetColour());
		rgbTotal[2] = GetBValue(vertex.GetColour());

		// Loops through all point light sources
		for (PointLight light : pointLights)
		{
			// Sets temp rgb values to light rgb intensity
			rgbTemp[0] = GetRValue(light.GetColour());
			rgbTemp[1] = GetGValue(light.GetColour());
			rgbTemp[2] = GetBValue(light.GetColour());

			// Modulates temp rgb by material reflectance coefficient 
			rgbTemp[0] *= _kPointDiffuse;
			rgbTemp[1] *= _kPointDiffuse;
			rgbTemp[2] *= _kPointDiffuse;

			// Gets attentuation value and applies it to rgb
			Vertex difference = vertex - light.GetPosition();
			Vertex normalisedDifference = difference.Normalise();
			float d = difference.Length();
			float atten = 1 / (light.GetA() + light.GetB() * d + light.GetC() * pow(d, 2));
			atten *= 100;

			rgbTemp[0] *= atten;
			rgbTemp[1] *= atten;
			rgbTemp[2] *= atten;

			// Gets dot product of of polygon and light sources normal vectors
			float dotProduct = normalisedDifference & vertex.GetNormal().Normalise();
			if (dotProduct < 0)
			{
				dotProduct = 0;
			}

			// Multiplies rgb values by dot product
			rgbTemp[0] *= dotProduct;
			rgbTemp[1] *= dotProduct;
			rgbTemp[2] *= dotProduct;

			// Add temp rgb to total rgb
			rgbTotal[0] += rgbTemp[0];
			rgbTotal[1] += rgbTemp[1];
			rgbTotal[2] += rgbTemp[2];
		}

		// Clamps rgb values between 0 and 255
		for (float& value : rgbTotal)
		{
			value = value <= 0 ? 0 : value <= 255 ? value : 255;
		}

		// Stored colour in vertex
		vertex.SetColour(RGB(rgbTotal[0], rgbTotal[1], rgbTotal[2]));
	}
}

// Applies directional specular lighting to each vertex in the model
void Model::CalculateSmoothLightingDirectionalSpecular(std::vector<DirectionalLight> directionalLights, Camera camera)
{
	float rgbTotal[3];
	float rgbTemp[3];

	// Loops through all vertices
	for (Vertex& vertex : _transformedVertices)
	{
		//Resets total rgb to current light
		rgbTotal[0] = GetRValue(vertex.GetColour());
		rgbTotal[1] = GetGValue(vertex.GetColour());
		rgbTotal[2] = GetBValue(vertex.GetColour());

		// Loops through all point light sources
		for (DirectionalLight light : directionalLights)
		{
			// Sets temp rgb values to light rgb intensity
			rgbTemp[0] = GetRValue(light.GetColour());
			rgbTemp[1] = GetGValue(light.GetColour());
			rgbTemp[2] = GetBValue(light.GetColour());

			Vertex L = light.GetDirection();
			Vertex N = vertex.GetNormal();
			Vertex V = vertex - camera.GetPosition();
			Vertex LPlusV = L.Normalise() + V.Normalise();
			Vertex H = LPlusV.Normalise();

			// Gets dot product of normal vector and light source direction
			float LDotN = L.Normalise() & N.Normalise();
			if (LDotN < 0)
			{
				LDotN = 0;
			}

			// Gets dot product of polygon normal vector and halfway vector
			float NDotH = N.Normalise() & H.Normalise();
			if (NDotH < 0)
			{
				NDotH = 0;
			}

			float iPD = (_kPointDiffuse * LDotN) + (_kPointSpecular * (pow(NDotH, _roughness)));

			// Multiplies rgb values by dot product
			rgbTemp[0] *= iPD;
			rgbTemp[1] *= iPD;
			rgbTemp[2] *= iPD;

			// Add temp rgb to total rgb
			rgbTotal[0] += rgbTemp[0];
			rgbTotal[1] += rgbTemp[1];
			rgbTotal[2] += rgbTemp[2];
		}

		// Clamps rgb values between 0 and 255
		for (float& value : rgbTotal)
		{
			value = value <= 0 ? 0 : value <= 255 ? value : 255;
		}

		// Stored colour in vertex
		vertex.SetColour(RGB(rgbTotal[0], rgbTotal[1], rgbTotal[2]));
	}
}

// Applies point specular lighting to each vertex in the model
void Model::CalculateSmoothLightingPointSpecular(std::vector<PointLight> pointLights, Camera camera)
{
	float rgbTotal[3];
	float rgbTemp[3];

	// Loops through all vertices
	for (Vertex& vertex : _transformedVertices)
	{
		//Resets total rgb to current light
		rgbTotal[0] = GetRValue(vertex.GetColour());
		rgbTotal[1] = GetGValue(vertex.GetColour());
		rgbTotal[2] = GetBValue(vertex.GetColour());

		// Loops through all point light sources
		for (PointLight light : pointLights)
		{
			// Sets temp rgb values to light rgb intensity
			rgbTemp[0] = GetRValue(light.GetColour());
			rgbTemp[1] = GetGValue(light.GetColour());
			rgbTemp[2] = GetBValue(light.GetColour());

			Vertex L = vertex - light.GetPosition();
			Vertex N = vertex.GetNormal();
			Vertex V = vertex - camera.GetPosition();
			Vertex LPlusV = L.Normalise() + V.Normalise();
			Vertex H = LPlusV.Normalise();

			// Gets dot product of normal vector and light source direction
			float LDotN = L.Normalise() & N.Normalise();
			if (LDotN < 0)
			{
				LDotN = 0;
			}

			// Gets dot product of polygon normal vector and halfway vector
			float NDotH = N.Normalise() & H.Normalise();
			if (NDotH < 0)
			{
				NDotH = 0;
			}

			// Gets attentuation value
			float d = L.Length();
			float atten = 1 / (light.GetA() + light.GetB() * d + light.GetC() * pow(d, 2));
			atten *= 100;

			float iPD = ((_kPointDiffuse * LDotN) * atten) + (_kPointSpecular * (pow((NDotH), _roughness)) * atten);

			// Multiplies rgb values by dot product
			rgbTemp[0] *= iPD;
			rgbTemp[1] *= iPD;
			rgbTemp[2] *= iPD;

			// Add temp rgb to total rgb
			rgbTotal[0] += rgbTemp[0];
			rgbTotal[1] += rgbTemp[1];
			rgbTotal[2] += rgbTemp[2];
		}

		// Clamps rgb values between 0 and 255
		for (float& value : rgbTotal)
		{
			value = value <= 0 ? 0 : value <= 255 ? value : 255;
		}

		// Stored colour in vertex
		vertex.SetColour(RGB(rgbTotal[0], rgbTotal[1], rgbTotal[2]));
	}
}

// Used to calculate light intensity for spot lights
float Model::SmoothStep(float edge0, float edge1, float x)
{
	float result;
	if (x < edge0)
	{
		result = 0.0f;
	}
	else if (x >= edge1)
	{
		result = 1.0f;
	}
	else
	{
		float t = (x - edge0) / (edge1 - edge0);
		result = (3.0f - 2.0f * t) * (t * t);
	}
	return result;
}

// Applies spot lighting to each vertex in the model
void Model::CalculateSpotLighting(std::vector<SpotLight> spotLights, Camera camera)
{
	float rgbTotal[3];
	float rgbTemp[3];

	// Loops through all vertices
	for (Vertex& vertex : _transformedVertices)
	{
		//Resets total rgb to current light
		rgbTotal[0] = GetRValue(vertex.GetColour());
		rgbTotal[1] = GetGValue(vertex.GetColour());
		rgbTotal[2] = GetBValue(vertex.GetColour());

		// Loops through all point light sources
		for (SpotLight light : spotLights)
		{
			// Sets temp rgb values to light rgb intensity
			rgbTemp[0] = GetRValue(light.GetColour());
			rgbTemp[1] = GetGValue(light.GetColour());
			rgbTemp[2] = GetBValue(light.GetColour());

			Vertex L = vertex - light.GetPosition();
			Vertex N = vertex.GetNormal();
			Vertex V = vertex - camera.GetPosition();
			Vertex LPlusV = L.Normalise() + V.Normalise();
			Vertex H = LPlusV.Normalise();

			// Gets dot product of normal vector and light source direction
			float LDotN = L.Normalise() & N.Normalise();
			if (LDotN < 0)
			{
				LDotN = 0;
			}

			// Gets dot product of polygon normal vector and halfway vector
			float NDotH = N.Normalise() & H.Normalise();
			if (NDotH < 0)
			{
				NDotH = 0;
			}

			// Gets attentuation value
			float d = L.Length();
			float atten = 1 / (light.GetA() + light.GetB() * d + light.GetC() * pow(d, 2));
			atten *= 100;

			float iPD = ((_kPointDiffuse * LDotN) * atten) + (_kPointSpecular * (pow((NDotH), _roughness)) * atten);

			// Gets smooothstep value to fade light between inner and outer angle
			float smoothstepVal = SmoothStep(cos(light.GetOuterAngle()), cos(light.GetInnerAngle()), LDotN);
			iPD *= smoothstepVal;

			// Multiplies rgb values by dot product
			rgbTemp[0] *= iPD;
			rgbTemp[1] *= iPD;
			rgbTemp[2] *= iPD;

			// Add temp rgb to total rgb
			rgbTotal[0] += rgbTemp[0];
			rgbTotal[1] += rgbTemp[1];
			rgbTotal[2] += rgbTemp[2];
		}

		// Clamps rgb values between 0 and 255
		for (float& value : rgbTotal)
		{
			value = value <= 0 ? 0 : value <= 255 ? value : 255;
		}

		// Stored colour in vertex
		vertex.SetColour(RGB(rgbTotal[0], rgbTotal[1], rgbTotal[2]));
	}
}

// Calculates the normal vectors at each vertex
void Model::CalculateNormals()
{
	for (Vertex& vertex : _transformedVertices)
	{
		vertex.SetNormal(Vertex(0, 0, 0));
		vertex.SetContributions(0);
	}

	for (Polygon3D& poly : _polygons)
	{
		// Gets vertices in polygon
		Vertex& vertex0 = _transformedVertices[poly.GetIndex(0)];
		Vertex& vertex1 = _transformedVertices[poly.GetIndex(1)];
		Vertex& vertex2 = _transformedVertices[poly.GetIndex(2)];

		// Gets difference between vertex0 and vertex1
		Vertex vectorA = vertex0 - vertex1;
		// Gets difference between vertex0 and vertex2
		Vertex vectorB = vertex0 - vertex2;

		// Calculates the normal vector
		Vertex normalVector = vectorB * vectorA;

		vertex0.SetNormal(vertex0.GetNormal() + normalVector);
		vertex1.SetNormal(vertex1.GetNormal() + normalVector);
		vertex2.SetNormal(vertex2.GetNormal() + normalVector);

		vertex0.IncreaseContributions();
		vertex1.IncreaseContributions();
		vertex2.IncreaseContributions();
	}

	for (Vertex& vertex : _transformedVertices)
	{
		float newx = vertex.GetNormal().GetX() / vertex.GetContributions();
		float newy = vertex.GetNormal().GetY() / vertex.GetContributions();
		float newz = vertex.GetNormal().GetZ() / vertex.GetContributions();
		Vertex temp = Vertex(newx, newy, newz);
		vertex.SetNormal(temp.Normalise());
	}
}



