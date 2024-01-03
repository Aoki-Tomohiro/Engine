#pragma once
#include "Engine/Math/Vector4.h"

class Color
{
public:
	float R() const { return value_.x; };
	float G() const { return value_.y; };
	float B() const { return value_.z; };
	float A() const { return value_.w; };

	void SetR(float r) { value_.x = r; };
	void SetG(float g) { value_.y = g; };
	void SetB(float b) { value_.z = b; };
	void SetA(float a) { value_.w = a; };

	bool operator==(const Color& rhs) const
	{
		return value_.x == rhs.value_.x &&
			value_.y == rhs.value_.y &&
			value_.z == rhs.value_.z &&
			value_.w == rhs.value_.w;
	};

	bool operator!=(const Color& rhs) const
	{
		return value_.x != rhs.value_.x &&
			value_.y != rhs.value_.y &&
			value_.z != rhs.value_.z &&
			value_.w != rhs.value_.w;
	}

	float* GetPtr() { return reinterpret_cast<float*>(this); };

	float& operator[](int index) { return GetPtr()[index]; };

	Color ToSRGB() const;

	Color FromSRGB() const;

private:
	static float SRGBComponent(float value);

	static float InverseSRGBComponent(float value);

private:
	Vector4 value_{ 0.1f, 0.25f, 0.5f, 1.0f };
};

