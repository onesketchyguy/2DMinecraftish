#pragma once

#ifndef BONUS_MATHS
#define BONUS_MATHS

struct float4
{
public:
	float x, y, z, w;

	float4(float x = 0, float y = 0, float z = 0, float w = 0)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

struct double4
{
public:
	double x, y, z, w;

	double4(double x = 0, double y = 0, double z = 0, double w = 0)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

struct int4
{
public:
	int x, y, z, w;

	int4(int x = 0, int y = 0, int z = 0, int w = 0)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

inline float Evaluate(int x, int y, int maxX, int maxY)
{
	olc::vf2d current{ static_cast<float>(x), static_cast<float>(y) };
	olc::vf2d max{ static_cast<float>(maxX), static_cast<float>(maxY) };
	olc::vf2d middle{ static_cast<float>(maxX) * 0.5f, static_cast<float>(maxY) * 0.5f };

	return ((middle - current) / max).mag();
}

inline float Evaluate(float value, float falloffPoint = 1.75f)
{
	if (value <= 0.001f) return 0;

	float a = 3;

	return pow(value, a) / (pow(value, a) + pow(falloffPoint - falloffPoint * value, a));
}

#endif