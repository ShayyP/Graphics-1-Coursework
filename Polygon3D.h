#pragma once
#include <Windows.h>
#include "Vertex.h"

class Polygon3D
{
public:
	// Constructors
	Polygon3D();
	Polygon3D(int index0, int index1, int index2, int uvIndex0, int uvIndex1, int uvIndex2);
	Polygon3D(const Polygon3D& p);
	// Destructor
	~Polygon3D();
	// Accessors
	int GetIndex(int) const;
	int GetUVIndex(int) const;
	void SetCulling(bool cull);
	bool GetCulling() const;
	void SetAverageZ(float average);
	float GetAverageZ() const;
	void SetColour(COLORREF colour);
	COLORREF GetColour() const;
	// Assingment operator
	Polygon3D& operator= (const Polygon3D& rhs);
	// Other operator
	const bool operator< (const Polygon3D& rhs) const;

private:
	int _indices[3];
	int _uvIndices[3];
	// Defines whether the polygon is backfacing
	bool _markedForCulling = false;
	float _averageZ;
	COLORREF _colour;
};


