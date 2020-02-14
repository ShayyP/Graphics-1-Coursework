#include "Rasteriser.h"

// Launches the program
Rasteriser app;

bool Rasteriser::Initialise()
{
	// Initialises variables
	_demo = Demo();
	// Defines camera
	_camera = Camera(0, 0, 0, Vertex(0, 0, -50));
	// Loads model
	return LoadModel(_demo.GetModel(), _demo.GetTexture());
}

// Loads model and textures from paths specified into _model
bool Rasteriser::LoadModel(const char* modelPath, const char* texturePath)
{
	_model = Model();
	// Loads model from md2 file, storing vertices and polygon indexes
	if (!MD2Loader::LoadModel(modelPath, texturePath, _model,
		&Model::AddPolygon,
		&Model::AddVertex,
		&Model::AddTextureUV))
	{
		return false;
	}

	return true;
}

// Returns viewing matrix to be applied to the model
Matrix Rasteriser::GenerateViewMatrix(Camera camera) 
{
	float ThetaX = camera.GetXRotation();
	Matrix matrix1 = Matrix({ 1, 0,            0,           0,
							  0, cos(ThetaX),  sin(ThetaX), 0,
							  0, -sin(ThetaX), cos(ThetaX), 0,
							  0, 0,            0,           1 });

	float ThetaY = camera.GetYRotation();
	Matrix matrix2 = Matrix({ cos(ThetaY), 0, -sin(ThetaY), 0,
							  0,           1, 0,            0,
							  sin(ThetaY), 0, cos(ThetaY),  0,
							  0,           0, 0,            1 });

	float ThetaZ = camera.GetZRotation();
	Matrix matrix3 = Matrix({ cos(ThetaZ),  sin(ThetaZ), 0, 0,
							  -sin(ThetaZ), cos(ThetaZ), 0, 0,
							  0,            0,           1, 0,
							  0,            0,           0, 1 });

	Vertex pos = camera.GetPosition();
	Matrix matrix4 = Matrix({ 1, 0, 0, -pos.GetX(),
							  0, 1, 0, -pos.GetY(),
							  0, 0, 1, -pos.GetZ(),
							  0, 0, 0, 1 });

	// Returns the result of multiplying the 4 matrices, this gives the view matrix
	return matrix1 * matrix2 * matrix3 * matrix4;
}

// Returns perspective matrix to be applied to the model
Matrix Rasteriser::GeneratePerspectiveMatrix(float d, float aspectRatio)
{
	return Matrix({ d / aspectRatio, 0, 0, 0,
				    0,               d, 0, 0,
				    0,               0, d, 0,
				    0,               0, 1, 0 });
}

// Returns screen matrix to be applied to the model
Matrix Rasteriser::GenerateScreenMatrix(float d, int width, int height)
{
	return Matrix({ float(width)/2, 0,                0,   float(width) /2,
		            0,              float(-height)/2, 0,   float(height)/2,
					0,              0,                d/2, d/2,
					0,              0,                0,   1 });
}

// Returns translation matrix using specified x, y and z translations
Matrix Rasteriser::GenerateTranslationMatrix(float x, float y, float z)
{
	return Matrix({ 1, 0, 0, x,
				   0, 1, 0, y,
				   0, 0, 1, z,
				   0, 0, 0, 1 });
}

// Returns translation matrix using specified scale
Matrix Rasteriser::GenerateScalingMatrix(float scale)
{
	return Matrix({ scale,       0,     0, 0,
					0,           scale, 0, 0,
					0,			 0,     scale, 0,
					0,			 0,     0, 1 });
}

// Returns translation matrix using x, y and z rotation values in radians
Matrix Rasteriser::GenerateRotationMatrix(float x, float y, float z)
{
	Matrix xMatrix = Matrix({ 1, 0,      0,       0,
							 0, cos(x), -sin(x), 0,
							 0, sin(x), cos(x),  0,
							 0, 0,      0,       1 });

	Matrix yMatrix = Matrix({ cos(y),  0, sin(y), 0,
							  0,       1, 0,      0,
							  -sin(y), 0, cos(y), 0,
						      0,       0, 0,      1 });

	Matrix zMatrix = Matrix({ cos(z), -sin(z), 0, 0,
							  sin(z), cos(z),  0, 0,
							  0,      0,       1, 0,
		                      0,      0,       0, 1, });

	return xMatrix * yMatrix * zMatrix;
}

// Output a string to the bitmap at co-ordinates 10, 10
// 
// Parameters: bitmap - A reference to the bitmap object
//             text   - A pointer to a string of wide characters
//
// For example, you might call this using:
//
//   DrawString(bitmap, L"Text to display");
void Rasteriser::DrawString(const Bitmap& bitmap, LPCTSTR text)
{
	HDC hdc = bitmap.GetDC();
	HFONT hFont, hOldFont;

	// Retrieve a handle to the variable stock font.  
	hFont = hFont = CreateFont(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Myfont"));

	// Select the variable stock font into the specified device context. 
	if (hOldFont = (HFONT)SelectObject(hdc, hFont))
	{
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(0, 0, 0));

		// Display the text string.  
		TextOut(hdc, 10, 10, text, lstrlen(text));

		// Restore the original font.        
		SelectObject(hdc, hOldFont);
	}
	DeleteObject(hFont);
}

// Updates model and applies tranformations
void Rasteriser::Update(const Bitmap& bitmap)
{
	// Updates demo class every frame
	_demo.Update();
	// If model has been changed, it is reloaded from md2 and pcx files
	if (_demo.GetChangedModel())
	{
		LoadModel(_demo.GetModel(), _demo.GetTexture());
		_demo.SetChangedModel(false);
	}

	// Applies model transformation
	_model.ApplyTransformToLocalVertices(GenerateTranslationMatrix(_demo.GetPosition(0), _demo.GetPosition(1), _demo.GetPosition(2)));
	_model.ApplyTransformToTransformedVertices(GenerateRotationMatrix(_demo.GetRotation(0), _demo.GetRotation(1), _demo.GetRotation(2)));
	_model.ApplyTransformToTransformedVertices(GenerateScalingMatrix(_demo.GetScale()));
}

// Draws model as wireframe
void Rasteriser::DrawWireframe(const Bitmap& bitmap, Polygon3D poly)
{
	// Creates white pen then selects it to be used
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	SelectObject(bitmap.GetDC(), pen);
	// Gets vertices that make up the polygon
	Vertex point0 = _model.GetTransformedVertices()[poly.GetIndex(0)];
	Vertex point1 = _model.GetTransformedVertices()[poly.GetIndex(1)];
	Vertex point2 = _model.GetTransformedVertices()[poly.GetIndex(2)];
	// Draws lines between each vertex, creating a triangle shape
	MoveToEx(bitmap.GetDC(), int(point0.GetX()), int(point0.GetY()), NULL);
	LineTo(bitmap.GetDC(), int(point1.GetX()), int(point1.GetY()));
	LineTo(bitmap.GetDC(), int(point2.GetX()), int(point2.GetY()));
	LineTo(bitmap.GetDC(), int(point0.GetX()), int(point0.GetY()));
	// Deletes the pen after use
	DeleteObject(pen);
}

// Draws model using windows polygons
void Rasteriser::DrawSolidFlat(const Bitmap& bitmap, Polygon3D poly)
{
	// Creates brush and pen of the polygon's colour then selects them to be used
	HBRUSH brush = CreateSolidBrush(poly.GetColour());
	HPEN pen = CreatePen(PS_SOLID, 1, poly.GetColour());
	SelectObject(bitmap.GetDC(), brush);
	SelectObject(bitmap.GetDC(), pen);

	// Gets vertices that make up the polygon
	Vertex vertex0 = _model.GetTransformedVertices()[poly.GetIndex(0)];
	Vertex vertex1 = _model.GetTransformedVertices()[poly.GetIndex(1)];
	Vertex vertex2 = _model.GetTransformedVertices()[poly.GetIndex(2)];

	// Creates an array of type POINT which is needed to use the Polygon function
	POINT points[3] = { POINT({long(vertex0.GetX()), long(vertex0.GetY())}), POINT({long(vertex1.GetX()), long(vertex1.GetY())}), POINT({long(vertex2.GetX()), long(vertex2.GetY())}) };
	// Draws the polygon to the screen
	Polygon(bitmap.GetDC(), points, 3);

	// Deletes the brush and pen after use
	DeleteObject(brush);
	DeleteObject(pen);
}

// Draws model using my own polygon function
void Rasteriser::MyDrawSolidFlat(const Bitmap& bitmap, Polygon3D poly)
{
	// Gets vertices that make up the polygon
	std::vector<Vertex> vertices;
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(0)]);
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(1)]);
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(2)]);

	// Sorts list of vertices in ascending order of Y values using lambda function
	std::sort(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) -> bool
	{
		return a.GetY() < b.GetY();
	});

	// Check for bottom flat triangle
	if (vertices[1].GetY() == vertices[2].GetY())
	{
		FillPolygonFlat(bitmap, vertices[0], vertices[1], vertices[2], poly.GetColour());
	}
	// Check for top flat triangle
	else if (vertices[0].GetY() == vertices[1].GetY())
	{
		FillPolygonFlat(bitmap, vertices[2], vertices[0], vertices[1], poly.GetColour());
	}
	// If not flat then split into two managable triangles
	else
	{
		// Gets intermediate vertex
		Vertex temp = Vertex(float(vertices[0].GetX() + (vertices[1].GetY() - vertices[0].GetY()) / (vertices[2].GetY() - vertices[0].GetY()) * (vertices[2].GetX() - vertices[0].GetX())), float(vertices[1].GetY()));
		FillPolygonFlat(bitmap, vertices[0], vertices[1], temp, poly.GetColour());
		FillPolygonFlat(bitmap, vertices[2], vertices[1], temp, poly.GetColour());
	}
}

// Returns -1 if input below 0, 0 if it is 0 and 1 if above 0
int Rasteriser::Signum(float x)
{
	return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

// Clamps value between 2 desired values
float Rasteriser::Clamp(float value, float lower, float upper)
{
	return value <= lower ? lower : value <= upper ? value : upper;
}

// Fills polygons using bresenham lines (flat shading)
void Rasteriser::FillPolygonFlat(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF colour)
{
	// Drawing the polygon using the Bresenham algorithm
	Vertex temp1 = Vertex(v1.GetX(), v1.GetY());
	Vertex temp2 = Vertex(v1.GetX(), v1.GetY());

	bool changed1 = false;
	bool changed2 = false;

	float dx1 = abs(v2.GetX() - v1.GetX());
	float dy1 = abs(v2.GetY() - v1.GetY());

	float dx2 = abs(v3.GetX() - v1.GetX());
	float dy2 = abs(v3.GetY() - v1.GetY());

	int signx1 = Signum(v2.GetX() - v1.GetX());
	int signx2 = Signum(v3.GetX() - v1.GetX());

	int signy1 = Signum(v2.GetY() - v1.GetY());
	int signy2 = Signum(v3.GetY() - v1.GetY());

	if (dy1 > dx1)
	{ 
		// Swap values
		float tmp = dx1;
		dx1 = dy1;
		dy1 = tmp;
		changed1 = true;
	}

	if (dy2 > dx2)
	{
		// Swap values
		float tmp = dx2;
		dx2 = dy2;
		dy2 = tmp;
		changed2 = true;
	}

	float e1 = 2 * dy1 - dx1;
	float e2 = 2 * dy2 - dx2;

	for (int i = 0; i <= dx1; i++)
	{
		float leftEndPoint;
		float rightEndPoint;

		if (temp1.GetX() < temp2.GetX())
		{
			leftEndPoint = temp1.GetX();
			rightEndPoint = temp2.GetX();
		}
		else
		{
			leftEndPoint = temp2.GetX();
			rightEndPoint = temp1.GetX();
		}

		for (int xPos = int(ceil(leftEndPoint)) - 1; xPos <= int(rightEndPoint) + 1; xPos++)
		{
			// Draws pixel
			SetPixel(bitmap.GetDC(), xPos, int(temp1.GetY()), colour);
		}

		while (e1 >= 0)
		{
			if (changed1)
			{
				temp1.SetX(temp1.GetX() + signx1);
			}
			else
			{
				temp1.SetY(temp1.GetY() + signy1);
			}

			e1 = e1 - 2 * dx1;
		}

		if (changed1)
		{
			temp1.SetY(temp1.GetY() + signy1);
		}
		else
		{
			temp1.SetX(temp1.GetX() + signx1);
		}

		e1 = e1 + 2 * dy1;

		while (temp2.GetY() != temp1.GetY())
		{
			while (e2 >= 0)
			{
				if (changed2)
				{
					temp2.SetX(temp2.GetX() + signx2);
				}
				else
				{
					temp2.SetY(temp2.GetY() + signy2);
				}

				e2 = e2 - 2 * dx2;
			}

			if (changed2)
			{
				temp2.SetY(temp2.GetY() + signy2);
			}
			else
			{
				temp2.SetX(temp2.GetX() + signx2);
			}

			e2 = e2 + 2 * dy2;
		}
	}
}

// Draws model using bresenham (smooth shading)
void Rasteriser::DrawGouraudBresenham(const Bitmap& bitmap, Polygon3D poly)
{
	// Gets vertices that make up the polygon
	std::vector<Vertex> vertices;
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(0)]);
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(1)]);
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(2)]);

	// Sorts list of vertices in ascending order of Y values using lambda function
	std::sort(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) -> bool
	{
		return a.GetY() < b.GetY();
	});

	Vertex v1 = vertices[0];
	Vertex v2 = vertices[1];
	Vertex v3 = vertices[2];

	// Check for bottom flat triangle
	if (v2.GetY() == v3.GetY())
	{
		FillPolygonGouraud(bitmap, v1, v2, v3, v1.GetColour(), v2.GetColour(), v3.GetColour());
	}
	// Check for top flat triangle
	else if (v1.GetY() == v2.GetY())
	{
		FillPolygonGouraud(bitmap, v3, v1, v2, v1.GetColour(), v2.GetColour(), v3.GetColour());
	}
	// If not flat then split into two managable triangles
	else
	{
		// Gets intermediate vertex
		Vertex vTemp = Vertex(v1.GetX() + (v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY()) * (v3.GetX() - v1.GetX()), v2.GetY());
		// Get intermediate colour values
		float cRed = GetRValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetRValue(v3.GetColour()) - GetRValue(v1.GetColour()));
		float cGreen = GetGValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetGValue(v3.GetColour()) - GetGValue(v1.GetColour()));
		float cBlue = GetBValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetBValue(v3.GetColour()) - GetBValue(v1.GetColour()));
		COLORREF cTemp = RGB(cRed, cGreen, cBlue);

		// As we have to draw each line from left to right, we have to check which point of the horizontal line has a lower x-coordinate and swap them if necessary
		if (v2.GetX() < vTemp.GetX())
		{
			FillPolygonGouraud(bitmap, v1, v2, vTemp, v1.GetColour(), v2.GetColour(), cTemp);
			FillPolygonGouraud(bitmap, v3, v2, vTemp, v3.GetColour(), v2.GetColour(), cTemp);
		}
		else
		{
			FillPolygonGouraud(bitmap, v1, vTemp, v2, v1.GetColour(), cTemp, v2.GetColour());
			FillPolygonGouraud(bitmap, v3, vTemp, v2, v3.GetColour(), cTemp, v2.GetColour());
		}
	}
}

// Fills polygon (smooth, bresenham)
void Rasteriser::FillPolygonGouraud(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3)
{
	// Drawing the polygon using the Bresenham algorithm
	Vertex temp1 = Vertex(v1.GetX(), v1.GetY());
	Vertex temp2 = Vertex(v1.GetX(), v1.GetY());

	bool changed1 = false;
	bool changed2 = false;

	float dx1 = abs(v2.GetX() - v1.GetX());
	float dy1 = abs(v2.GetY() - v1.GetY());

	// Get total color change on edge1
	int dRed1 = GetRValue(c2) - GetRValue(c1);
	int dGreen1 = GetGValue(c2) - GetGValue(c1);
	int dBlue1 = GetBValue(c2) - GetBValue(c1);

	float dx2 = abs(v3.GetX() - v1.GetX());
	float dy2 = abs(v3.GetY() - v1.GetY());

	// Get total color change on edge2
	int dRed2 = GetRValue(c3) - GetRValue(c1);
	int dGreen2 = GetGValue(c3) - GetGValue(c1);
	int dBlue2 = GetBValue(c3) - GetBValue(c1);

	int signx1 = Signum(v2.GetX() - v1.GetX());
	int signx2 = Signum(v3.GetX() - v1.GetX());

	int signy1 = Signum(v2.GetY() - v1.GetY());
	int signy2 = Signum(v3.GetY() - v1.GetY());

	// Get 'direction' of color change on edge2
	int signRed2 = Signum(float(GetRValue(c3) - GetRValue(c1)));
	int signGreen2 = Signum(float(GetGValue(c3) - GetGValue(c1)));
	int signBlue2 = Signum(float(GetBValue(c3) - GetBValue(c1)));

	if (dy1 > dx1)
	{
		// Swap values
		float tmp = dx1;
		dx1 = dy1;
		dy1 = tmp;
		changed1 = true;
	}

	if (dy2 > dx2)
	{
		// Swap values
		float tmp = dx2;
		dx2 = dy2;
		dy2 = tmp;
		changed2 = true;
	}

	float e1 = 2 * dy1 - dx1;
	float e2 = 2 * dy2 - dx2;

	// Get scaled starting color of edge1
	float red1 = GetRValue(c1) * dx1;
	float green1 = GetGValue(c1) * dx1;
	float blue1 = GetBValue(c1) * dx1;

	// Get scaled starting color of edge2
	float red2 = GetRValue(c1) * dx1;
	float green2 = GetGValue(c1) * dx1;
	float blue2 = GetBValue(c1) * dx1;

	// Get color change of each step for edge1
	int deltaRed1 = dRed1;
	int deltaGreen1 = dGreen1;
	int deltaBlue1 = dBlue1;

	// Get color change of each step for edge2
	int deltaRed2 = dRed2;
	int deltaGreen2 = dGreen2;
	int deltaBlue2 = dBlue2;

	for (int i = 0; i <= dx1; i++)
	{
		// Loop over each pixel of horizontal line 
		// Check if paint goes from left endpoint of horizontal line to right or other way round

		float leftEndPoint;
		float rightEndPoint;

		if (temp1.GetX() < temp2.GetX())
		{
			leftEndPoint = temp1.GetX();
			rightEndPoint = temp2.GetX();
		}
		else
		{
			leftEndPoint = temp2.GetX();
			rightEndPoint = temp1.GetX();
		}

		for (int xPos = int(ceil(leftEndPoint))-1; xPos <= int(rightEndPoint)+1; xPos++)
		{
			float scale = xPos - leftEndPoint;
			float diff = rightEndPoint - leftEndPoint + 1;

			float redTmp = (diff - scale) * red1 + scale * red2;
			float greenTmp = (diff - scale) * green1 + scale * green2;
			float blueTmp = (diff - scale) * blue1 + scale * blue2;

			COLORREF colour = RGB(redTmp / (diff * dx1), greenTmp / (diff * dx1), blueTmp / (diff * dx1));

			// Draws pixel
			SetPixel(bitmap.GetDC(), xPos, int(temp1.GetY()), colour);
		}

		while (e1 >= 0)
		{
			if (changed1)
			{
				temp1.SetX(temp1.GetX() + signx1);
			}
			else
			{
				temp1.SetY(temp1.GetY() + signy1);
			}

			e1 = e1 - 2 * dx1;
		}

		if (changed1)
		{
			temp1.SetY(temp1.GetY() + signy1);
		}
		else
		{
			temp1.SetX(temp1.GetX() + signx1);
		}

		e1 = e1 + 2 * dy1;

		// Here we rendered the next point on line 1 so follow now line 2
		// Until we are on the same y-value as line 1.
		while (temp2.GetY() != temp1.GetY())
		{
			while (e2 >= 0)
			{
				if (changed2)
				{
					temp2.SetX(temp2.GetX() + signx2);
				}
				else
				{
					temp2.SetY(temp2.GetY() + signy2);
				}

				e2 = e2 - 2 * dx2;

				red2 += signRed2;
				blue2 += signBlue2;
				green2 += signGreen2;
			}

			if (changed2)
			{
				temp2.SetY(temp2.GetY() + signy2);
			}
			else
			{
				temp2.SetX(temp2.GetX() + signx2);
			}

			e2 = e2 + 2 * dy2;
		}

		// Increase the color values at each edge
		red1 += deltaRed1;
		green1 += deltaGreen1;
		blue1 += deltaBlue1;

		red2 += deltaRed2;
		green2 += deltaGreen2;
		blue2 += deltaBlue2;
	}
}

// Draws model using standard algorithm (smooth shading) - used Bresenham in demo as it seems to produce a better result
void Rasteriser::DrawGouraudStandard(const Bitmap& bitmap, Polygon3D poly)
{
	// Gets vertices that make up the polygon
	std::vector<Vertex> vertices;
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(0)]);
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(1)]);
	vertices.push_back(_model.GetTransformedVertices()[poly.GetIndex(2)]);

	// Sorts list of vertices in ascending order of Y values using lambda function
	std::sort(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) -> bool
	{
		return a.GetY() < b.GetY();
	});

	Vertex v1 = vertices[0];
	Vertex v2 = vertices[1];
	Vertex v3 = vertices[2];

	// Check for bottom flat triangle
	if (v2.GetY() == v3.GetY())
	{
		FillBottomGouraud(bitmap, v1, v2, v3, v1.GetColour(), v2.GetColour(), v3.GetColour());
	}
	// Check for top flat triangle
	else if (v1.GetY() == v2.GetY())
	{
		FillTopGouraud(bitmap, v1, v2, v3, v1.GetColour(), v2.GetColour(), v3.GetColour());
	}
	// If not flat then split into two managable triangles
	else
	{
		Vertex vTemp = Vertex(v1.GetX() + (v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY()) * (v3.GetX() - v1.GetX()), v2.GetY());

		// Get intermediate colour values
		float cRed = GetRValue(v1.GetColour()) + ((float)(v2.GetY() - v1.GetY()) / (float)(v3.GetY() - v1.GetY())) * (GetRValue(v3.GetColour()) - GetRValue(v1.GetColour()));
		float cGreen = GetGValue(v1.GetColour()) + ((float)(v2.GetY() - v1.GetY()) / (float)(v3.GetY() - v1.GetY())) * (GetGValue(v3.GetColour()) - GetGValue(v1.GetColour()));
		float cBlue = GetBValue(v1.GetColour()) + ((float)(v2.GetY() - v1.GetY()) / (float)(v3.GetY() - v1.GetY())) * (GetBValue(v3.GetColour()) - GetBValue(v1.GetColour()));

		COLORREF cTemp = RGB(cRed, cGreen, cBlue);

		FillBottomGouraud(bitmap, v1, v2, vTemp, v1.GetColour(), v2.GetColour(), cTemp);
		FillTopGouraud(bitmap, v2, vTemp, v3, v2.GetColour(), cTemp, v3.GetColour());
	}
}

// Fills bottom flat polygon (smooth, standard) - used Bresenham in demo as it seems to produce a better result
void Rasteriser::FillBottomGouraud(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3)
{
	float slope1 = (v2.GetX() - v1.GetX()) / (v2.GetY() - v1.GetY());
	float slope2 = (v3.GetX() - v1.GetX()) / (v3.GetY() - v1.GetY());

	float x1 = v1.GetX();
	float x2 = v1.GetX() + 0.5f;

	float v2v1Diff = v2.GetY() - v1.GetY();
	float colourSlopeRed1 = (GetRValue(c2) - GetRValue(c1)) / v2v1Diff;
	float colourSlopeGreen1 = (GetGValue(c2) - GetGValue(c1)) / v2v1Diff;
	float colourSlopeBlue1 = (GetBValue(c2) - GetBValue(c1)) / v2v1Diff;

	float v3v1Diff = v3.GetY() - v1.GetY();
	float colourSlopeRed2 = (GetRValue(c3) - GetRValue(c1)) / v3v1Diff;
	float colourSlopeGreen2 = (GetGValue(c3) - GetGValue(c1)) / v3v1Diff;
	float colourSlopeBlue2 = (GetBValue(c3) - GetBValue(c1)) / v3v1Diff;

	float cRed1 = GetRValue(c1);
	float cGreen1 = GetGValue(c1);
	float cBlue1 = GetBValue(c1);
	float cRed2 = GetRValue(c1);
	float cGreen2 = GetGValue(c1);
	float cBlue2 = GetBValue(c1);

	if (slope2 < slope1)
	{
		float slopeTemp = slope1;
		slope1 = slope2;
		slope2 = slopeTemp;

		slopeTemp = colourSlopeRed1;
		colourSlopeRed1 = colourSlopeRed2;
		colourSlopeRed2 = slopeTemp;

		slopeTemp = colourSlopeGreen1;
		colourSlopeGreen1 = colourSlopeGreen2;
		colourSlopeGreen2 = slopeTemp;

		slopeTemp = colourSlopeBlue1;
		colourSlopeBlue1 = colourSlopeBlue2;
		colourSlopeBlue2 = slopeTemp;
	}

	for (int scanlineY = int(v1.GetY()); scanlineY <= v2.GetY(); scanlineY++)
	{
		for (int xPos = int(ceil(x1)); xPos <= int(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);

			int red = int((1 - t) * cRed1 + t * cRed2);
			int green = int((1 - t) * cGreen1 + t * cGreen2);
			int blue = int((1 - t) * cBlue1 + t * cBlue2);

			// Draws pixel
			SetPixel(bitmap.GetDC(), xPos, scanlineY, RGB(red, green, blue));
		}

		x1 += slope1;
		x2 += slope2;

		cRed1 += colourSlopeRed1;
		cGreen1 += colourSlopeGreen1;
		cBlue1 += colourSlopeBlue1;

		cRed2 += colourSlopeRed2;
		cGreen2 += colourSlopeGreen2;
		cBlue2 += colourSlopeBlue2;
	}
}

// Fills top flat polygon (smooth, standard) - used Bresenham in demo as it seems to produce a better result
void Rasteriser::FillTopGouraud(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3)
{
	float slope1 = (v3.GetX() - v1.GetX()) / (v3.GetY() - v1.GetY());
	float slope2 = (v3.GetX() - v2.GetX()) / (v3.GetY() - v2.GetY());

	float x1 = v3.GetX();
	float x2 = v3.GetX() + 0.5f;

	float v3v1Diff = v3.GetY() - v1.GetY();
	float colourSlopeRed1 = (GetRValue(c3) - GetRValue(c1)) / v3v1Diff;
	float colourSlopeGreen1 = (GetGValue(c3) - GetGValue(c1)) / v3v1Diff;
	float colourSlopeBlue1 = (GetBValue(c3) - GetBValue(c1)) / v3v1Diff;

	float v3v2Diff = v3.GetY() - v2.GetY();
	float colourSlopeRed2 = (GetRValue(c3) - GetRValue(c2)) / v3v2Diff;
	float colourSlopeGreen2 = (GetGValue(c3) - GetGValue(c2)) / v3v2Diff;
	float colourSlopeBlue2 = (GetBValue(c3) - GetBValue(c2)) / v3v2Diff;

	float cRed1 = GetRValue(c3);
	float cGreen1 = GetGValue(c3);
	float cBlue1 = GetBValue(c3);
	float cRed2 = GetRValue(c3);
	float cGreen2 = GetGValue(c3);
	float cBlue2 = GetBValue(c3);

	if (slope1 < slope2)
	{
		float slopeTemp = slope1;
		slope1 = slope2;
		slope2 = slopeTemp;

		slopeTemp = colourSlopeRed1;
		colourSlopeRed1 = colourSlopeRed2;
		colourSlopeRed2 = slopeTemp;

		slopeTemp = colourSlopeGreen1;
		colourSlopeGreen1 = colourSlopeGreen2;
		colourSlopeGreen2 = slopeTemp;

		slopeTemp = colourSlopeBlue1;
		colourSlopeBlue1 = colourSlopeBlue2;
		colourSlopeBlue2 = slopeTemp;
	}

	for (int scanlineY = int(v3.GetY()); scanlineY >= v1.GetY(); scanlineY--)
	{
		for (int xPos = int(ceil(x1)); xPos <= int(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);

			int red = int((1 - t) * cRed1 + t * cRed2);
			int green = int((1 - t) * cGreen1 + t * cGreen2);
			int blue = int((1 - t) * cBlue1 + t * cBlue2);

			// Draws pixel
			SetPixel(bitmap.GetDC(), xPos, scanlineY, RGB(red, green, blue));
		}

		x1 -= slope1;
		x2 -= slope2;

		cRed1 -= colourSlopeRed1;
		cGreen1 -= colourSlopeGreen1;
		cBlue1 -= colourSlopeBlue1;

		cRed2 -= colourSlopeRed2;
		cGreen2 -= colourSlopeGreen2;
		cBlue2 -= colourSlopeBlue2;
	}
}

// Draws model using bresenham (smooth shading & textures)
void Rasteriser::DrawGouraudTextured(const Bitmap& bitmap, Polygon3D poly)
{
	Vertex v1 = _model.GetTransformedVertices()[poly.GetIndex(0)];
	Vertex v2 = _model.GetTransformedVertices()[poly.GetIndex(1)];
	Vertex v3 = _model.GetTransformedVertices()[poly.GetIndex(2)];
	// Sets UV indices for vertices
	v1.SetUVIndex(poly.GetUVIndex(0));
	v2.SetUVIndex(poly.GetUVIndex(1));
	v3.SetUVIndex(poly.GetUVIndex(2));
	// Sorts list of vertices in ascending order of Y values using lambda function polygon
	std::vector<Vertex> vertices;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	std::sort(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) -> bool
	{
		return a.GetY() < b.GetY();
	});

	v1 = vertices[0];
	v2 = vertices[1];
	v3 = vertices[2];

	// Get UV pairs for vertices
	UVPair v1UV = _model.GetUVPairs()[v1.GetUVIndex()];
	UVPair v2UV = _model.GetUVPairs()[v2.GetUVIndex()];
	UVPair v3UV = _model.GetUVPairs()[v3.GetUVIndex()];

	// Check for bottom flat triangle
	if (v2.GetY() == v3.GetY())
	{
		FillGouraudTextured(bitmap, v1, v2, v3, v1.GetColour(), v2.GetColour(), v3.GetColour(), v1UV, v2UV, v3UV);
	}
	// Check for top flat triangle
	else if (v1.GetY() == v2.GetY())
	{
		FillGouraudTextured(bitmap, v3, v1, v2, v1.GetColour(), v2.GetColour(), v3.GetColour(), v1UV, v2UV, v3UV);
	}
	// If not flat then split into two managable triangles
	else
	{
		Vertex vTemp = Vertex(v1.GetX() + (v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY()) * (v3.GetX() - v1.GetX()), v2.GetY());

		// Get intermediate colour values
		float cRed = GetRValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetRValue(v3.GetColour()) - GetRValue(v1.GetColour()));
		float cGreen = GetGValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetGValue(v3.GetColour()) - GetGValue(v1.GetColour()));
		float cBlue = GetBValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetBValue(v3.GetColour()) - GetBValue(v1.GetColour()));

		COLORREF cTemp = RGB(cRed, cGreen, cBlue);

		// Get intermediate UV values
		float uvTempU = v1UV.GetU() + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * v3UV.GetU() - v1UV.GetU();
		float uvTempV = v1UV.GetV() + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * v3UV.GetV() - v1UV.GetV();
		
		UVPair uvTemp = UVPair(uvTempU, uvTempV);

		// As we have to draw each line from left to right, we have to check which point of the horizontal line has a lower x-coordinate and swap them if necessary
		if (v2.GetX() < vTemp.GetX())
		{
			FillGouraudTextured(bitmap, v1, v2, vTemp, v1.GetColour(), v2.GetColour(), cTemp, v1UV, v2UV, uvTemp);
			FillGouraudTextured(bitmap, v3, v2, vTemp, v3.GetColour(), v2.GetColour(), cTemp, v3UV, v2UV, uvTemp);
		}
		else
		{
			FillGouraudTextured(bitmap, v1, vTemp, v2, v1.GetColour(), cTemp, v2.GetColour(), v1UV, uvTemp, v2UV);
			FillGouraudTextured(bitmap, v3, vTemp, v2, v3.GetColour(), cTemp, v2.GetColour(), v3UV, uvTemp, v2UV);
		}
	}
}

// Fills polygon (smooth, bresenham & textures)
void Rasteriser::FillGouraudTextured(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3, UVPair uv1, UVPair uv2, UVPair uv3)
{
	// Drawing the polygon using the Bresenham algorithm
	Vertex temp1 = Vertex(v1.GetX(), v1.GetY());
	Vertex temp2 = Vertex(v1.GetX(), v1.GetY());

	bool changed1 = false;
	bool changed2 = false;

	float dx1 = abs(v2.GetX() - v1.GetX());
	float dy1 = abs(v2.GetY() - v1.GetY());

	int dRed1 = GetRValue(c2) - GetRValue(c1);
	int dGreen1 = GetGValue(c2) - GetGValue(c1);
	int dBlue1 = GetBValue(c2) - GetBValue(c1);

	float dU1 = uv2.GetU() - uv1.GetU();
	float dV1 = uv2.GetV() - uv1.GetV();

	float dx2 = abs(v3.GetX() - v1.GetX());
	float dy2 = abs(v3.GetY() - v1.GetY());

	int dRed2 = GetRValue(c3) - GetRValue(c1);
	int dGreen2 = GetGValue(c3) - GetGValue(c1);
	int dBlue2 = GetBValue(c3) - GetBValue(c1);

	float dU2 = uv3.GetU() - uv1.GetU();
	float dV2 = uv3.GetV() - uv1.GetV();

	int signx1 = Signum(v2.GetX() - v1.GetX());
	int signx2 = Signum(v3.GetX() - v1.GetX());

	int signy1 = Signum(v2.GetY() - v1.GetY());
	int signy2 = Signum(v3.GetY() - v1.GetY());

	int signRed2 = Signum(float(GetRValue(c3) - GetRValue(c1)));
	int signGreen2 = Signum(float(GetGValue(c3) - GetGValue(c1)));
	int signBlue2 = Signum(float(GetBValue(c3) - GetBValue(c1)));

	int signU2 = Signum(uv3.GetU() - uv1.GetU());
	int signV2 = Signum(uv3.GetV() - uv1.GetV());

	if (dy1 > dx1)
	{
		float tmp = dx1;
		dx1 = dy1;
		dy1 = tmp;
		changed1 = true;
	}

	if (dy2 > dx2)
	{
		float tmp = dx2;
		dx2 = dy2;
		dy2 = tmp;
		changed2 = true;
	}

	float e1 = 2 * dy1 - dx1;
	float e2 = 2 * dy2 - dx2;

	float red1 = GetRValue(c1) * dx1;
	float green1 = GetGValue(c1) * dx1;
	float blue1 = GetBValue(c1) * dx1;

	float U1 = uv1.GetU() * dx1;
	float V1 = uv1.GetV() * dx1;

	float red2 = GetRValue(c1) * dx1;
	float green2 = GetGValue(c1) * dx1;
	float blue2 = GetBValue(c1) * dx1;

	float U2 = uv1.GetU() * dx1;
	float V2 = uv1.GetV() * dx1;

	int deltaRed1 = dRed1;
	int deltaGreen1 = dGreen1;
	int deltaBlue1 = dBlue1;

	float deltaU1 = dU1;
	float deltaV1 = dV1;

	int deltaRed2 = dRed2;
	int deltaGreen2 = dGreen2;
	int deltaBlue2 = dBlue2;

	float deltaU2 = dU2;
	float deltaV2 = dV2;

	for (int i = 0; i < dx1; i++)
	{
		float leftEndPoint;
		float rightEndPoint;

		if (temp1.GetX() <= temp2.GetX())
		{
			leftEndPoint = temp1.GetX();
			rightEndPoint = temp2.GetX();
		}
		else
		{
			leftEndPoint = temp2.GetX();
			rightEndPoint = temp1.GetX();
		}

		for (int xPos = int(ceil(leftEndPoint)) - 1; xPos <= int(rightEndPoint) + 1; xPos++)
		{
			float scale = xPos - leftEndPoint;
			float diff = rightEndPoint - leftEndPoint + 1;

			float redTmp = (diff - scale) * red1 + scale * red2;
			float greenTmp = (diff - scale) * green1 + scale * green2;
			float blueTmp = (diff - scale) * blue1 + scale * blue2;

			float uTmp = (diff - scale) * U1 + scale * U2;
			float vTmp = (diff - scale) * V1 + scale * V2;

			float lightRed = redTmp / (diff * dx1);
			float lightGreen = greenTmp / (diff * dx1);
			float lightBlue = blueTmp / (diff * dx1);

			float textureU = uTmp / (diff * dx1);
			float textureV = vTmp / (diff * dx1);

			// Getting RGB value of texture using interpolated U and V values
			COLORREF textureColour = _model.GetTexture().GetTextureValue(int(textureU), int(textureV));

			// Multiplies texture colours by light multipliers
			float finalRed = GetRValue(textureColour) * (lightRed / 255);
			float finalGreen = GetGValue(textureColour) * (lightGreen / 255);
			float finalBlue = GetBValue(textureColour) * (lightBlue / 255);

			// Clamps colour between 0 and 255
			COLORREF colour = RGB(Clamp(finalRed, 0, 255), Clamp(finalGreen, 0, 255), Clamp(finalBlue, 0, 255));

			// Draws pixel
			SetPixel(bitmap.GetDC(), xPos, int(temp1.GetY()), colour);
		}

		while (e1 >= 0)
		{
			if (changed1)
			{
				temp1.SetX(temp1.GetX() + signx1);
			}
			else
			{
				temp1.SetY(temp1.GetY() + signy1);
			}

			e1 = e1 - 2 * dx1;
		}

		if (changed1)
		{
			temp1.SetY(temp1.GetY() + signy1);
		}
		else
		{
			temp1.SetX(temp1.GetX() + signx1);
		}

		e1 = e1 + 2 * dy1;

		while (temp2.GetY() != temp1.GetY())
		{
			while (e2 >= 0)
			{
				if (changed2)
				{
					temp2.SetX(temp2.GetX() + signx2);
				}
				else
				{
					temp2.SetY(temp2.GetY() + signy2);
				}

				e2 = e2 - 2 * dx2;

				red2 += signRed2;
				blue2 += signBlue2;
				green2 += signGreen2;
				U2 += signU2;
				V2 += signV2;
			}

			if (changed2)
			{
				temp2.SetY(temp2.GetY() + signy2);
			}
			else
			{
				temp2.SetX(temp2.GetX() + signx2);
			}

			e2 = e2 + 2 * dy2;
		}

		red1 += deltaRed1;
		green1 += deltaGreen1;
		blue1 += deltaBlue1;
		U1 += deltaU1;
		V1 += deltaV1;

		red2 += deltaRed2;
		green2 += deltaGreen2;
		blue2 += deltaBlue2;
		U2 += deltaU2;
		V2 += deltaV2;
	}
}

// The following 5 functions draw corrected textures, attempted with standard and bresenham algorithm but can't get either to work
// Bresenham version is shown at the end of the demo. Code left to show what I have attempted as it looks correct to me and Wayne said he could't see an obvious issue
void Rasteriser::DrawTexturedCorrectedBresenham(const Bitmap& bitmap, Polygon3D poly)
{
	Vertex v1 = _model.GetTransformedVertices()[poly.GetIndex(0)];
	Vertex v2 = _model.GetTransformedVertices()[poly.GetIndex(1)];
	Vertex v3 = _model.GetTransformedVertices()[poly.GetIndex(2)];
	// Sets UV indices for vertices
	v1.SetUVIndex(poly.GetUVIndex(0));
	v2.SetUVIndex(poly.GetUVIndex(1));
	v3.SetUVIndex(poly.GetUVIndex(2));
	// Sorts list of vertices in ascending order of Y values using lambda function polygon
	std::vector<Vertex> vertices;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	std::sort(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) -> bool
	{
		return a.GetY() < b.GetY();
	});

	v1 = vertices[0];
	v2 = vertices[1];
	v3 = vertices[2];

	// Get UV pairs for vertices
	UVPair v1UV = _model.GetUVPairs()[v1.GetUVIndex()];
	UVPair v2UV = _model.GetUVPairs()[v2.GetUVIndex()];
	UVPair v3UV = _model.GetUVPairs()[v3.GetUVIndex()];

	// Set UOverZ, VoverZ and ZRecip for UV pairs
	v1UV.SetUOverZ(v1UV.GetU() / v1.GetPreTransformZ());
	v1UV.SetVOverZ(v1UV.GetV() / v1.GetPreTransformZ());
	v1UV.SetZRecip(1 / v1.GetPreTransformZ());

	v2UV.SetUOverZ(v2UV.GetU() / v2.GetPreTransformZ());
	v2UV.SetVOverZ(v2UV.GetV() / v2.GetPreTransformZ());
	v2UV.SetZRecip(1 / v2.GetPreTransformZ());

	v3UV.SetUOverZ(v3UV.GetU() / v3.GetPreTransformZ());
	v3UV.SetVOverZ(v3UV.GetV() / v3.GetPreTransformZ());
	v3UV.SetZRecip(1 / v3.GetPreTransformZ());

	// Check for bottom flat triangle
	if (v2.GetY() == v3.GetY())
	{
		FillTexturedCorrected(bitmap, v1, v2, v3, v1.GetColour(), v2.GetColour(), v3.GetColour(), v1UV, v2UV, v3UV);
	}
	// Check for top flat triangle
	else if (v1.GetY() == v2.GetY())
	{
		FillTexturedCorrected(bitmap, v3, v1, v2, v1.GetColour(), v2.GetColour(), v3.GetColour(), v1UV, v2UV, v3UV);
	}
	// If not flat then split into two managable triangles
	else
	{
		Vertex vTemp = Vertex(v1.GetX() + (v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY()) * (v3.GetX() - v1.GetX()), v2.GetY());

		// Get intermediate colour values
		float cRed = GetRValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetRValue(v3.GetColour()) - GetRValue(v1.GetColour()));
		float cGreen = GetGValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetGValue(v3.GetColour()) - GetGValue(v1.GetColour()));
		float cBlue = GetBValue(v1.GetColour()) + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * (GetBValue(v3.GetColour()) - GetBValue(v1.GetColour()));

		COLORREF cTemp = RGB(cRed, cGreen, cBlue);

		// Get intermediate UV values
		float uvTempU = v1UV.GetU() + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * v3UV.GetU() - v1UV.GetU();
		float uvTempV = v1UV.GetV() + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * v3UV.GetV() - v1UV.GetV();

		UVPair uvTemp = UVPair(uvTempU, uvTempV);

		// Get intermediate z value
		float zTemp = v1.GetPreTransformZ() + ((v2.GetPreTransformZ() - v1.GetPreTransformZ()) / (v3.GetPreTransformZ() - v1.GetPreTransformZ())) * v3.GetPreTransformZ() - v1.GetPreTransformZ();
		// Set UOverZ, VOverZ and ZRecip for the temp UV pair
		uvTemp.SetUOverZ(uvTempU / zTemp);
		uvTemp.SetVOverZ(uvTempV / zTemp);
		uvTemp.SetZRecip(1 / zTemp);

		// As we have to draw each line from left to right, we have to check which point of the horizontal line has a lower x-coordinate and swap them if necessary
		if (v2.GetX() < vTemp.GetX())
		{
			FillTexturedCorrected(bitmap, v1, v2, vTemp, v1.GetColour(), v2.GetColour(), cTemp, v1UV, v2UV, uvTemp);
			FillTexturedCorrected(bitmap, v3, v2, vTemp, v3.GetColour(), v2.GetColour(), cTemp, v3UV, v2UV, uvTemp);
		}
		else
		{
			FillTexturedCorrected(bitmap, v1, vTemp, v2, v1.GetColour(), cTemp, v2.GetColour(), v1UV, uvTemp, v2UV);
			FillTexturedCorrected(bitmap, v3, vTemp, v2, v3.GetColour(), cTemp, v2.GetColour(), v3UV, uvTemp, v2UV);
		}
	}
}

void Rasteriser::FillTexturedCorrected(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3, UVPair uv1, UVPair uv2, UVPair uv3)
{
	// Drawing the polygon using the Bresenham algorithm
	Vertex temp1 = Vertex(v1.GetX(), v1.GetY());
	Vertex temp2 = Vertex(v1.GetX(), v1.GetY());

	bool changed1 = false;
	bool changed2 = false;

	float dx1 = abs(v2.GetX() - v1.GetX());
	float dy1 = abs(v2.GetY() - v1.GetY());

	int dRed1 = GetRValue(c2) - GetRValue(c1);
	int dGreen1 = GetGValue(c2) - GetGValue(c1);
	int dBlue1 = GetBValue(c2) - GetBValue(c1);

	float dUoverZ1 = uv2.GetUOverZ() - uv1.GetUOverZ();
	float dVoverZ1 = uv2.GetVOverZ() - uv1.GetVOverZ();
	float dZRecip1 = uv2.GetZRecip() - uv1.GetZRecip();

	float dx2 = abs(v3.GetX() - v1.GetX());
	float dy2 = abs(v3.GetY() - v1.GetY());

	int dRed2 = GetRValue(c3) - GetRValue(c1);
	int dGreen2 = GetGValue(c3) - GetGValue(c1);
	int dBlue2 = GetBValue(c3) - GetBValue(c1);

	float dUoverZ2 = uv3.GetUOverZ() - uv1.GetUOverZ();
	float dVoverZ2 = uv3.GetVOverZ() - uv1.GetVOverZ();
	float dZRecip2 = uv3.GetZRecip() - uv1.GetZRecip();

	int signx1 = Signum(v2.GetX() - v1.GetX());
	int signx2 = Signum(v3.GetX() - v1.GetX());

	int signy1 = Signum(v2.GetY() - v1.GetY());
	int signy2 = Signum(v3.GetY() - v1.GetY());

	int signRed2 = Signum(float(GetRValue(c3) - GetRValue(c1)));
	int signGreen2 = Signum(float(GetGValue(c3) - GetGValue(c1)));
	int signBlue2 = Signum(float(GetBValue(c3) - GetBValue(c1)));

	int signUoverZ2 = Signum(uv3.GetUOverZ() - uv1.GetUOverZ());
	int signVoverZ2 = Signum(uv3.GetVOverZ() - uv1.GetVOverZ());
	int signZRecip2 = Signum(uv3.GetZRecip() - uv1.GetZRecip());

	if (dy1 > dx1)
	{
		float tmp = dx1;
		dx1 = dy1;
		dy1 = tmp;
		changed1 = true;
	}

	if (dy2 > dx2)
	{
		float tmp = dx2;
		dx2 = dy2;
		dy2 = tmp;
		changed2 = true;
	}

	float e1 = 2 * dy1 - dx1;
	float e2 = 2 * dy2 - dx2;

	float red1 = GetRValue(c1) * dx1;
	float green1 = GetGValue(c1) * dx1;
	float blue1 = GetBValue(c1) * dx1;

	float uOverZ1 = uv1.GetUOverZ() * dx1;
	float vOverZ1 = uv1.GetVOverZ() * dx1;
	float zRecip1 = uv1.GetZRecip() * dx1;

	float red2 = GetRValue(c1) * dx1;
	float green2 = GetGValue(c1) * dx1;
	float blue2 = GetBValue(c1) * dx1;

	float uOverZ2 = uv1.GetUOverZ() * dx1;
	float vOverZ2 = uv1.GetVOverZ() * dx1;
	float zRecip2 = uv1.GetZRecip() * dx1;

	int deltaRed1 = dRed1;
	int deltaGreen1 = dGreen1;
	int deltaBlue1 = dBlue1;

	float deltaUOverZ1 = dUoverZ1;
	float deltaVOverZ1 = dVoverZ1;
	float deltaZRecip1 = dZRecip1;

	int deltaRed2 = dRed2;
	int deltaGreen2 = dGreen2;
	int deltaBlue2 = dBlue2;

	float deltaUOverZ2 = dUoverZ2;
	float deltaVOverZ2 = dVoverZ2;
	float deltaZRecip2 = dZRecip2;

	for (int i = 0; i <= dx1; i++)
	{
		float leftEndPoint;
		float rightEndPoint;

		if (temp1.GetX() < temp2.GetX())
		{
			leftEndPoint = temp1.GetX();
			rightEndPoint = temp2.GetX();
		}
		else
		{
			leftEndPoint = temp2.GetX();
			rightEndPoint = temp1.GetX();
		}

		for (int xPos = int(ceil(leftEndPoint)) - 1; xPos <= int(rightEndPoint) + 1; xPos++)
		{
			float scale = xPos - leftEndPoint;
			float diff = rightEndPoint - leftEndPoint + 1;

			float redTmp = (diff - scale) * red1 + scale * red2;
			float greenTmp = (diff - scale) * green1 + scale * green2;
			float blueTmp = (diff - scale) * blue1 + scale * blue2;

			float uOverZTmp = (diff - scale) * uOverZ1 + scale * uOverZ2;
			float vOverZTmp = (diff - scale) * vOverZ1 + scale * vOverZ2;
			float zRecipTmp = (diff - scale) * zRecip1 + scale * zRecip2;

			float lightRed = redTmp / (diff * dx1);
			float lightGreen = greenTmp / (diff * dx1);
			float lightBlue = blueTmp / (diff * dx1);

			float textureUOverZ = uOverZTmp / (diff * dx1);
			float textureVOverZ = vOverZTmp / (diff * dx1);
			float textureZRecip = zRecipTmp / (diff * dx1);

			// Gets texture colour using interpolated UOverZ, VOverZ and ZRecip values
			COLORREF textureColour = _model.GetTexture().GetTextureValue(int(textureUOverZ / textureZRecip), int(textureVOverZ / textureZRecip));

			// Applies lighting to the texture colour
			float finalRed = GetRValue(textureColour) * (lightRed / 255);
			float finalGreen = GetGValue(textureColour) * (lightGreen / 255);
			float finalBlue = GetBValue(textureColour) * (lightBlue / 255);

			// Clamps the final colour between 0 and 255
			COLORREF colour = RGB(Clamp(finalRed, 0, 255), Clamp(finalGreen, 0, 255), Clamp(finalBlue, 0, 255));

			// Draws pixel
			SetPixel(bitmap.GetDC(), xPos, int(temp1.GetY()), colour);
		}

		while (e1 >= 0)
		{
			if (changed1)
			{
				temp1.SetX(temp1.GetX() + signx1);
			}
			else
			{
				temp1.SetY(temp1.GetY() + signy1);
			}

			e1 = e1 - 2 * dx1;
		}

		if (changed1)
		{
			temp1.SetY(temp1.GetY() + signy1);
		}
		else
		{
			temp1.SetX(temp1.GetX() + signx1);
		}

		e1 = e1 + 2 * dy1;

		while (temp2.GetY() != temp1.GetY())
		{
			while (e2 >= 0)
			{
				if (changed2)
				{
					temp2.SetX(temp2.GetX() + signx2);
				}
				else
				{
					temp2.SetY(temp2.GetY() + signy2);
				}

				e2 = e2 - 2 * dx2;

				red2 += signRed2;
				blue2 += signBlue2;
				green2 += signGreen2;
				uOverZ2 += signUoverZ2;
				vOverZ2 += signVoverZ2;
				zRecip2 += signZRecip2;
			}

			if (changed2)
			{
				temp2.SetY(temp2.GetY() + signy2);
			}
			else
			{
				temp2.SetX(temp2.GetX() + signx2);
			}

			e2 = e2 + 2 * dy2;
		}

		red1 += deltaRed1;
		green1 += deltaGreen1;
		blue1 += deltaBlue1;
		uOverZ1 += deltaUOverZ1;
		vOverZ1 += deltaVOverZ1;
		zRecip1 += deltaZRecip1;

		red2 += deltaRed2;
		green2 += deltaGreen2;
		blue2 += deltaBlue2;
		uOverZ2 += deltaUOverZ2;
		vOverZ2 += deltaVOverZ2;
		zRecip2 += deltaZRecip2;
	}
}

void Rasteriser::DrawTexturedCorrectedStandard(const Bitmap& bitmap, Polygon3D poly)
{
	Vertex v1 = _model.GetTransformedVertices()[poly.GetIndex(0)];
	Vertex v2 = _model.GetTransformedVertices()[poly.GetIndex(1)];
	Vertex v3 = _model.GetTransformedVertices()[poly.GetIndex(2)];
	// Sets UV indices for vertices
	v1.SetUVIndex(poly.GetUVIndex(0));
	v2.SetUVIndex(poly.GetUVIndex(1));
	v3.SetUVIndex(poly.GetUVIndex(2));
	// Sorts list of vertices in ascending order of Y values using lambda function polygon
	std::vector<Vertex> vertices;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	std::sort(vertices.begin(), vertices.end(), [](const Vertex& a, const Vertex& b) -> bool
	{
		return a.GetY() < b.GetY();
	});

	v1 = vertices[0];
	v2 = vertices[1];
	v3 = vertices[2];

	v1 = vertices[0];
	v2 = vertices[1];
	v3 = vertices[2];

	// Get UV pairs for vertices
	UVPair v1UV = _model.GetUVPairs()[v1.GetUVIndex()];
	UVPair v2UV = _model.GetUVPairs()[v2.GetUVIndex()];
	UVPair v3UV = _model.GetUVPairs()[v3.GetUVIndex()];

	// Set UOverZ, VoverZ and ZRecip for UV pairs
	v1UV.SetUOverZ(v1UV.GetU() / v1.GetPreTransformZ());
	v1UV.SetVOverZ(v1UV.GetV() / v1.GetPreTransformZ());
	v1UV.SetZRecip(1 / v1.GetPreTransformZ());

	v2UV.SetUOverZ(v2UV.GetU() / v2.GetPreTransformZ());
	v2UV.SetVOverZ(v2UV.GetV() / v2.GetPreTransformZ());
	v2UV.SetZRecip(1 / v2.GetPreTransformZ());

	v3UV.SetUOverZ(v3UV.GetU() / v3.GetPreTransformZ());
	v3UV.SetVOverZ(v3UV.GetV() / v3.GetPreTransformZ());
	v3UV.SetZRecip(1 / v3.GetPreTransformZ());

	// Check for bottom flat triangle
	if (v2.GetY() == v3.GetY())
	{
		FillBottomTextured(bitmap, v1, v2, v3, v1.GetColour(), v2.GetColour(), v3.GetColour(), v1UV, v2UV, v3UV);
	}
	// Check for top flat triangle
	else if (v1.GetY() == v2.GetY())
	{
		FillTopTextured(bitmap, v1, v2, v3, v1.GetColour(), v2.GetColour(), v3.GetColour(), v1UV, v2UV, v3UV);
	}
	// If not flat then split into two managable triangles
	else
	{
		Vertex vTemp = Vertex(v1.GetX() + (v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY()) * (v3.GetX() - v1.GetX()), v2.GetY());

		// Get intermediate colour values
		float cRed = GetRValue(v1.GetColour()) + ((float)(v2.GetY() - v1.GetY()) / (float)(v3.GetY() - v1.GetY())) * (GetRValue(v3.GetColour()) - GetRValue(v1.GetColour()));
		float cGreen = GetGValue(v1.GetColour()) + ((float)(v2.GetY() - v1.GetY()) / (float)(v3.GetY() - v1.GetY())) * (GetGValue(v3.GetColour()) - GetGValue(v1.GetColour()));
		float cBlue = GetBValue(v1.GetColour()) + ((float)(v2.GetY() - v1.GetY()) / (float)(v3.GetY() - v1.GetY())) * (GetBValue(v3.GetColour()) - GetBValue(v1.GetColour()));

		COLORREF cTemp = RGB(cRed, cGreen, cBlue);

		// Get intermediate UV values
		float uvTempU = v1UV.GetU() + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * v3UV.GetU() - v1UV.GetU();
		float uvTempV = v1UV.GetV() + ((v2.GetY() - v1.GetY()) / (v3.GetY() - v1.GetY())) * v3UV.GetV() - v1UV.GetV();

		UVPair uvTemp = UVPair(uvTempU, uvTempV);

		// Get intermediate z value
		float zTemp = v1.GetPreTransformZ() + ((v2.GetPreTransformZ() - v1.GetPreTransformZ()) / (v3.GetPreTransformZ() - v1.GetPreTransformZ())) * v3.GetPreTransformZ() - v1.GetPreTransformZ();
		// Set UOverZ, VOverZ and ZRecip for the temp UV pair
		uvTemp.SetUOverZ(uvTempU / zTemp);
		uvTemp.SetVOverZ(uvTempV / zTemp);
		uvTemp.SetZRecip(1 / zTemp);

		FillBottomTextured(bitmap, v1, v2, vTemp, v1.GetColour(), v2.GetColour(), cTemp, v1UV, v2UV, uvTemp);
		FillTopTextured(bitmap, v2, vTemp, v3, v2.GetColour(), cTemp, v3.GetColour(), v2UV, uvTemp, v3UV);
	}
}

void Rasteriser::FillBottomTextured(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3, UVPair uv1, UVPair uv2, UVPair uv3)
{
	float slope1 = (v2.GetX() - v1.GetX()) / (v2.GetY() - v1.GetY());
	float slope2 = (v3.GetX() - v1.GetX()) / (v3.GetY() - v1.GetY());

	float x1 = v1.GetX();
	float x2 = v1.GetX() + 0.5f;

	float v2v1Diff = v2.GetY() - v1.GetY();
	float colourSlopeRed1 = (GetRValue(c2) - GetRValue(c1)) / v2v1Diff;
	float colourSlopeGreen1 = (GetGValue(c2) - GetGValue(c1)) / v2v1Diff;
	float colourSlopeBlue1 = (GetBValue(c2) - GetBValue(c1)) / v2v1Diff;
	float uOverZSlope1 = (uv2.GetU() - uv1.GetU()) / v2v1Diff;
	float vOverZSlope1 = (uv2.GetV() - uv1.GetV()) / v2v1Diff;
	float zRecipSlope1 = (uv2.GetZRecip() - uv1.GetZRecip()) / v2v1Diff;

	float v3v1Diff = v3.GetY() - v1.GetY();
	float colourSlopeRed2 = (GetRValue(c3) - GetRValue(c1)) / v3v1Diff;
	float colourSlopeGreen2 = (GetGValue(c3) - GetGValue(c1)) / v3v1Diff;
	float colourSlopeBlue2 = (GetBValue(c3) - GetBValue(c1)) / v3v1Diff;
	float uOverZSlope2 = (uv3.GetU() - uv1.GetU()) / v3v1Diff;
	float vOverZSlope2 = (uv3.GetV() - uv1.GetV()) / v3v1Diff;
	float zRecipSlope2 = (uv3.GetZRecip() - uv1.GetZRecip()) / v3v1Diff;

	float cRed1 = GetRValue(c1);
	float cGreen1 = GetGValue(c1);
	float cBlue1 = GetBValue(c1);
	float cUOverZ1 = uv1.GetU();
	float cVOverZ1 = uv1.GetV();
	float cZRecip1 = uv1.GetZRecip();

	float cRed2 = GetRValue(c1);
	float cGreen2 = GetGValue(c1);
	float cBlue2 = GetBValue(c1);
	float cUOverZ2 = uv1.GetU();
	float cVOverZ2 = uv1.GetV();
	float cZRecip2 = uv1.GetZRecip();

	if (slope2 < slope1)
	{
		float slopeTemp = slope1;
		slope1 = slope2;
		slope2 = slopeTemp;

		slopeTemp = colourSlopeRed1;
		colourSlopeRed1 = colourSlopeRed2;
		colourSlopeRed2 = slopeTemp;

		slopeTemp = colourSlopeGreen1;
		colourSlopeGreen1 = colourSlopeGreen2;
		colourSlopeGreen2 = slopeTemp;

		slopeTemp = colourSlopeBlue1;
		colourSlopeBlue1 = colourSlopeBlue2;
		colourSlopeBlue2 = slopeTemp;

		slopeTemp = uOverZSlope1;
		uOverZSlope1 = uOverZSlope2;
		uOverZSlope2 = slopeTemp;

		slopeTemp = vOverZSlope1;
		vOverZSlope1 = vOverZSlope2;
		vOverZSlope2 = slopeTemp;

		slopeTemp = zRecipSlope1;
		zRecipSlope1 = zRecipSlope2;
		zRecipSlope2 = slopeTemp;
	}

	for (int scanlineY = int(v1.GetY()); scanlineY <= v2.GetY(); scanlineY++)
	{
		for (int xPos = int(ceil(x1)); xPos <= int(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);

			int red = int((1 - t) * cRed1 + t * cRed2);
			int green = int((1 - t) * cGreen1 + t * cGreen2);
			int blue = int((1 - t) * cBlue1 + t * cBlue2);
			float uOverZ = (1 - t) * cUOverZ1 + t * cUOverZ2;
			float vOverZ = (1 - t) * cVOverZ1 + t * cVOverZ2;
			float zRecip = (1 - t) * cZRecip1 + t * cZRecip2;

			COLORREF textureColour = _model.GetTexture().GetTextureValue(int(uOverZ / zRecip), int(vOverZ / zRecip));

			float finalRed = float(GetRValue(textureColour) * (red / 255));
			float finalGreen = float(GetGValue(textureColour) * (green / 255));
			float finalBlue = float(GetBValue(textureColour) * (blue / 255));

			COLORREF colour = RGB(Clamp(finalRed, 0, 255), Clamp(finalGreen, 0, 255), Clamp(finalBlue, 0, 255));

			// Draws pixel
			SetPixel(bitmap.GetDC(), xPos, scanlineY, colour);
		}

		x1 += slope1;
		x2 += slope2;

		cRed1 += colourSlopeRed1;
		cGreen1 += colourSlopeGreen1;
		cBlue1 += colourSlopeBlue1;
		cUOverZ1 += uOverZSlope1;
		cVOverZ1 += vOverZSlope1;
		cZRecip1 += zRecipSlope1;

		cRed2 += colourSlopeRed2;
		cGreen2 += colourSlopeGreen2;
		cBlue2 += colourSlopeBlue2;
		cUOverZ2 += uOverZSlope2;
		cVOverZ2 += vOverZSlope2;
		cZRecip2 += zRecipSlope2;
	}
}

void Rasteriser::FillTopTextured(const Bitmap& bitmap, Vertex v1, Vertex v2, Vertex v3, COLORREF c1, COLORREF c2, COLORREF c3, UVPair uv1, UVPair uv2, UVPair uv3)
{
	float slope1 = (v3.GetX() - v1.GetX()) / (v3.GetY() - v1.GetY());
	float slope2 = (v3.GetX() - v2.GetX()) / (v3.GetY() - v2.GetY());

	float x1 = v3.GetX();
	float x2 = v3.GetX() + 0.5f;

	float v3v1Diff = v3.GetY() - v1.GetY();
	float colourSlopeRed1 = (GetRValue(c3) - GetRValue(c1)) / v3v1Diff;
	float colourSlopeGreen1 = (GetGValue(c3) - GetGValue(c1)) / v3v1Diff;
	float colourSlopeBlue1 = (GetBValue(c3) - GetBValue(c1)) / v3v1Diff;
	float uOverZSlope1 = (uv3.GetU() - uv1.GetU()) / v3v1Diff;
	float vOverZSlope1 = (uv3.GetV() - uv1.GetV()) / v3v1Diff;
	float zRecipSlope1 = (uv3.GetZRecip() - uv1.GetZRecip()) / v3v1Diff;

	float v3v2Diff = v3.GetY() - v2.GetY();
	float colourSlopeRed2 = (GetRValue(c3) - GetRValue(c2)) / v3v2Diff;
	float colourSlopeGreen2 = (GetGValue(c3) - GetGValue(c2)) / v3v2Diff;
	float colourSlopeBlue2 = (GetBValue(c3) - GetBValue(c2)) / v3v2Diff;
	float uOverZSlope2 = (uv3.GetU() - uv2.GetU()) / v3v2Diff;
	float vOverZSlope2 = (uv3.GetV() - uv2.GetV()) / v3v2Diff;
	float zRecipSlope2 = (uv3.GetZRecip() - uv1.GetZRecip()) / v3v2Diff;

	float cRed1 = GetRValue(c3);
	float cGreen1 = GetGValue(c3);
	float cBlue1 = GetBValue(c3);
	float cUOverZ1 = uv3.GetU();
	float cVOverZ1 = uv3.GetV();
	float cZRecip1 = uv3.GetZRecip();

	float cRed2 = GetRValue(c3);
	float cGreen2 = GetGValue(c3);
	float cBlue2 = GetBValue(c3);
	float cUOverZ2 = uv3.GetU();
	float cVOverZ2 = uv3.GetV();
	float cZRecip2 = uv3.GetZRecip();

	if (slope1 < slope2)
	{
		float slopeTemp = slope1;
		slope1 = slope2;
		slope2 = slopeTemp;

		slopeTemp = colourSlopeRed1;
		colourSlopeRed1 = colourSlopeRed2;
		colourSlopeRed2 = slopeTemp;

		slopeTemp = colourSlopeGreen1;
		colourSlopeGreen1 = colourSlopeGreen2;
		colourSlopeGreen2 = slopeTemp;

		slopeTemp = colourSlopeBlue1;
		colourSlopeBlue1 = colourSlopeBlue2;
		colourSlopeBlue2 = slopeTemp;

		slopeTemp = uOverZSlope1;
		uOverZSlope1 = uOverZSlope2;
		uOverZSlope2 = slopeTemp;

		slopeTemp = vOverZSlope1;
		vOverZSlope1 = vOverZSlope2;
		vOverZSlope2 = slopeTemp;

		slopeTemp = zRecipSlope1;
		zRecipSlope1 = zRecipSlope2;
		zRecipSlope2 = slopeTemp;
	}

	for (int scanlineY = int(v3.GetY()); scanlineY >= v1.GetY(); scanlineY--)
	{
		for (int xPos = int(ceil(x1)); xPos <= int(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);

			int red = int((1 - t) * cRed1 + t * cRed2);
			int green = int((1 - t) * cGreen1 + t * cGreen2);
			int blue = int((1 - t) * cBlue1 + t * cBlue2);
			float uOverZ = (1 - t) * cUOverZ1 + t * cUOverZ2;
			float vOverZ = (1 - t) * cVOverZ1 + t * cVOverZ2;
			float zRecip = (1 - t) * cZRecip1 + t * cZRecip2;

			COLORREF textureColour = _model.GetTexture().GetTextureValue(int(uOverZ / zRecip), int(vOverZ / zRecip));

			float finalRed = float(GetRValue(textureColour) * (red / 255));
			float finalGreen = float(GetGValue(textureColour) * (green / 255));
			float finalBlue = float(GetBValue(textureColour) * (blue / 255));

			COLORREF colour = RGB(Clamp(finalRed, 0, 255), Clamp(finalGreen, 0, 255), Clamp(finalBlue, 0, 255));

			// Draws pixel
			SetPixel(bitmap.GetDC(), xPos, scanlineY, colour);
		}

		x1 -= slope1;
		x2 -= slope2;

		cRed1 -= colourSlopeRed1;
		cGreen1 -= colourSlopeGreen1;
		cBlue1 -= colourSlopeBlue1;
		cUOverZ1 -= uOverZSlope1;
		cVOverZ1 -= vOverZSlope1;
		cZRecip1 -= zRecipSlope1;

		cRed2 -= colourSlopeRed2;
		cGreen2 -= colourSlopeGreen2;
		cBlue2 -= colourSlopeBlue2;
		cUOverZ2 -= uOverZSlope2;
		cVOverZ2 -= vOverZSlope2;
		cZRecip2 -= zRecipSlope2;
	}
}

// Rendering pipeline, applies required tranformations and draws model to the screen
void Rasteriser::Render(const Bitmap& bitmap)
{
	// Gets size of bitmap
	int windowWidth = bitmap.GetWidth();
	int windowHeight = bitmap.GetHeight();

	// Calculates backfaces and marks polygons for culling (if at that stage in demo)
	if (_demo.GetBackface())
	{	
		_model.CalculateBackfaces(_camera);
	}

	// Calculates flat lighting
	if (!_demo.GetSmoothShading())
	{
		// Applies ambient lighting to the model
		_model.CalculateFlatLightingAmbient(_demo.GetAmbientLight());

		// Applies directional lighting to the model
		_model.CalculateFlatLightingDirectional(_demo.GetDirectionalLights());

		// Applies point lighting to the model
		_model.CalculateFlatLightingPoint(_demo.GetPointLights());
	}
	else
	{
		// Calculates normals for each vertex
		_model.CalculateNormals();
		// Calculates smooth lighting
		if (!_demo.GetSpecular())
		{
			// Applies ambient lighting to the model
			_model.CalculateSmoothLightingAmbient(_demo.GetAmbientLight());

			// Applies directional lighting to the model
			_model.CalculateSmoothLightingDirectional(_demo.GetDirectionalLights());

			// Applies point lighting to the model
			_model.CalculateSmoothLightingPoint(_demo.GetPointLights());
		}
		// Calculates smooth, specular lighting
		else
		{
			// Applies ambient lighting to the model
			_model.CalculateSmoothLightingAmbient(_demo.GetAmbientLight());

			// Applies directional lighting to the model
			_model.CalculateSmoothLightingDirectionalSpecular(_demo.GetDirectionalLights(), _camera);

			// Applies point lighting to the model
			_model.CalculateSmoothLightingPointSpecular(_demo.GetPointLights(), _camera);

			// Applies spot lighting to the model
			_model.CalculateSpotLighting(_demo.GetSpotLights(), _camera);
		}
	}

	// Applies Viewing transformation
	_model.ApplyTransformToTransformedVertices(GenerateViewMatrix(_camera));

	// Applies Perspective/Projection transformation
	_model.ApplyTransformToTransformedVertices(GeneratePerspectiveMatrix(1, float(windowWidth) / float(windowHeight)));

	// Sorts polygons in the model so that polygons further from the camera are drawn first
	_model.Sort();

	// Dehomogenises the vertices
	_model.Dehomogenise();

	// Applies Screen tranformation
	_model.ApplyTransformToTransformedVertices(GenerateScreenMatrix(1, windowWidth, windowHeight));

	// Clear the bitmap to black
	bitmap.Clear(RGB(0, 0, 0));

	//Gets draw mode from demo class
	std::string drawMode = _demo.GetDrawMode();

	// Loops through all polygons in the model
	for (Polygon3D poly : _model.GetPolygons()) 
	{
		// Polygon is only drawn if it is not marked for culling
		if (!poly.GetCulling())
		{
			// Uses drawing function that is specified by the demo class
			if (drawMode == "Wireframe")
			{
				DrawWireframe(bitmap, poly);
			}
			else if (drawMode == "Solid")
			{
				DrawSolidFlat(bitmap, poly);
			}
			else if (drawMode == "MySolid")
			{
				MyDrawSolidFlat(bitmap, poly);
			}
			else if (drawMode == "Bresenham")
			{
				DrawGouraudBresenham(bitmap, poly);
			}
			else if (drawMode == "Textured")
			{
				DrawGouraudTextured(bitmap, poly);
			}
			else if (drawMode == "TexturedCorrected")
			{
				DrawTexturedCorrectedBresenham(bitmap, poly);
			}
		}
	}
	// Gets stage from demo class and displays it on screen
	std::string stage = _demo.GetStage();
	// wstring(stage.begin(), stage.end()).c_str() converts string to a widestring which is required for the DrawString function
	DrawString(bitmap, wstring(stage.begin(), stage.end()).c_str());
}
