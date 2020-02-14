#pragma once

// Class to store U and V values for texturing
class UVPair
{
public:
	// Constructors and destructor
	UVPair();
	UVPair(float u, float v);
	UVPair(const UVPair& p);
	~UVPair();
	// Accessors and mutators
	float GetU() const;
	void SetU(float u);
	float GetV() const;
	void SetV(float v);
	float GetUOverZ() const;
	void SetUOverZ(float UOverZ);
	float GetVOverZ() const;
	void SetVOverZ(float VOverZ);
	float GetZRecip() const;
	void SetZRecip(float ZRecip);

private:
	float _u;
	float _v;
	float _uOverZ;
	float _vOverZ;
	float _zRecip;
};

