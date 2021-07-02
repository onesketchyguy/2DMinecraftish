// Forrest Lowe 2021

#pragma once
#ifndef OBJECT_DEFINITIONS
#define OBJECT_DEFINITIONS

struct LightSource
{
public:
	olc::vf2d position;
	olc::Pixel color;
	float intensity;

	void Initialize(olc::vf2d position, olc::Pixel color, float intensity)
	{
		this->position = position;
		this->color = color;
		this->intensity = intensity;
	}
};

#endif