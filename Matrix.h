#pragma once
#include "Vertex.h"

// Class for a 4 x 4 matrix object.  

const int ROWS = 4;
const int COLS = 4;

#include <initializer_list>

class Matrix
{
public:
	// Default constructor
	Matrix();

	// Constructor that initialises all elements.  Note that an initialiser list is used
	// to pass in the values.  This is a new C++ feature.
	Matrix(std::initializer_list<float> list);

	// Copy constructor
	Matrix(const Matrix& other) noexcept;

	// Destructor
	~Matrix() noexcept;

	// Retrieve value in matrix at specified row and column
	float GetM(const int row, const int column) const;

	// Set value in matrix at specified row and column
	void SetM(const int row, const int column, const float value);

	// Copy assignment operator
	Matrix& operator= (const Matrix& rhs) noexcept;

	// Test for two matrices being equivalent;
	bool operator==(const Matrix& other) const;

	// Multiply two matrices together
	const Matrix operator*(const Matrix& other) const;

	// Multiply a matrix by a vertex, returning a vertex
	const Vertex operator*(const Vertex& other) const;

	static Matrix IdentityMatrix();

private:
	float _m[ROWS][COLS];

	// Private method to copy contents of one matrix
	// to another
	void Copy(const Matrix& other);
};

