// Forrest Lowe 2021

#pragma once

#ifndef WORLDDATA_H
#define WORLDDATA_H

#include "olcPixelGameEngine.h"
#include "ConstantData.h"
#include "debugger.h"

class WorldData
{
private:
	const uint8_t MAP_WIDTH = 255;
	const uint8_t MAP_HEIGHT = 255;

	const float NOISE_SCALE = 10.0f;

	uint16_t mapLength = 0;
	// 2D noise variables
	int outputLength;
	float* noiseSeed = nullptr;
	float* perlinNoise = nullptr;

	int nOctaveCount = 6;
	float fScalingBias = 1.3f;

	bool worldGenerated = false;
	uint8_t generatingWorld = 255;
	uint8_t generatingFalloff = 255;

	void PerlinNoise2D(int nWidth, int nHeight, int nOctaves,
		float fBias, float* output);
	float Evaluate(float value, float falloffPoint = 1.75f);
	void ReseedNoise();
	void ApplySeed(std::string seedString);
	void InitNoise();

public:
	WorldData()
	{
		mapLength = MAP_WIDTH * MAP_HEIGHT;
	}

public:
	uint8_t* tileData = nullptr;
	uint8_t* foliageData = nullptr;

	void GenerateMap();
	bool GetWorldGenerated();
	float GetWorldProgress();
	float GetFalloffProgress();
	int GetMapWidth();
	int GetMapHeight();
	int GetTileID(int x, int y);
	int GetTileID(float x, float y);
	int GetFoliageID(int x, int y);
	int GetFoliageID(float x, float y);
	int GetFoliageIndex(float fx, float fy);
	void SetTileID(float x, float y, uint8_t value);
	void SetFoliageID(float x, float y, uint8_t value);
	olc::vf2d GetRandomGroundTile();
};

#endif // !WORLDDATA_H