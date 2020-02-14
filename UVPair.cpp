#include "UVPair.h"

// Default constructor 
UVPair::UVPair()
{
	_u = 0.0f;
	_v = 0.0f;
	_uOverZ = 0.0f;
	_vOverZ = 0.0f;
	_zRecip = 0.0f;
}

// Other constructor 
UVPair::UVPair(float u, float v)
{
	_u = u;
	_v = v;
	_uOverZ = 0.0f;
	_vOverZ = 0.0f;
	_zRecip = 0.0f;
}

// Copy constructor
UVPair::UVPair(const UVPair& p)
{
	_u = p.GetU();
	_v = p.GetV();
	_uOverZ = p.GetUOverZ();
	_vOverZ = p.GetVOverZ();
	_zRecip = p.GetZRecip();
}

// Destructor
UVPair::~UVPair()
{
}

// Accessors and mutators
float UVPair::GetU() const
{
	return _u;
}

void UVPair::SetU(float u)
{
	_u = u;
}

float UVPair::GetV() const
{
	return _v;
}

void UVPair::SetV(float v)
{
	_v = v;
}

float UVPair::GetUOverZ() const
{
	return _uOverZ;
}

void UVPair::SetUOverZ(float UOverZ)
{
	_uOverZ = UOverZ;
}

float UVPair::GetVOverZ() const
{
	return _vOverZ;
}

void UVPair::SetVOverZ(float VOverZ)
{
	_vOverZ = VOverZ;
}

float UVPair::GetZRecip() const
{
	return _zRecip;
}

void UVPair::SetZRecip(float ZRecip)
{
	_zRecip = ZRecip;
}
