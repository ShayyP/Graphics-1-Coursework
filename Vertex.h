#pragma once
#include <math.h>
#include <Windows.h>

class Vertex
{
public:
	// Constructors
	Vertex();
	Vertex(float x, float y);
	Vertex(float x, float y, float z);
	Vertex(float x, float y, float z, float w);
	Vertex(const Vertex& other);
	// Destructor
	~Vertex();

	// Accessors and mutators
	float GetX() const;
	void SetX(const float x);
	float GetY() const;
	void SetY(const float y);
	float GetZ() const;
	void SetZ(const float z);
	float GetW() const;
	void SetW(const float w);
	Vertex GetNormal() const;
	void SetNormal(const Vertex normal);
	COLORREF GetColour() const;
	void SetColour(const COLORREF colour);
	int GetContributions() const;
	void IncreaseContributions();
	void SetContributions(const int cont);
	int GetUVIndex() const;
	void SetUVIndex(const int index);
	float GetPreTransformZ() const;
	void SavePreTransformZ();

	// Other methods
	void Dehomogenise();
	Vertex Normalise();
	float Length();

	// Assignment operator
	Vertex& operator= (const Vertex& rhs);

	//Other operators
	bool operator== (const Vertex& rhs) const;
	const Vertex operator+ (const Vertex& rhs) const;
	const Vertex operator- (const Vertex& rhs) const;
	const Vertex operator* (const Vertex& rhs) const;
	const float operator& (const Vertex& rhs) const;

private:
	// Position
	float _x;
	float _y;
	float _z;
	float _w;

	// Variables used in shading
	float _normalX;
	float _normalY;
	float _normalZ;
	COLORREF _colour;
	int _contributions;

	// UV index used to get texture at vertex
	int _uvIndex;
	// Pre transform Z used in texture correction, saved before dehomogenisation
	float _preTransformZ;
};

