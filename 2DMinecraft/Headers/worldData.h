// Forrest Lowe 2021

#pragma once

#ifndef WORLDDATA_H
#define WORLDDATA_H

#include "olcPixelGameEngine.h"
#include "ConstantData.h"
#include "debugger.h"

struct WorldGenerationData
{
public:
	float* fallOffMapA = nullptr;
	float* fallOffMapB = nullptr;

	float* perlinNoise = nullptr;
	float* noiseSeed = nullptr;

	bool worldSeeded = false;

public:
	WorldGenerationData(int mapLength)
	{
		float* fallOffMapA = new float[mapLength];
		float* fallOffMapB = new float[mapLength];

		float* perlinNoise = new float[mapLength];
		float* noiseSeed = new float[mapLength];
	}

	~WorldGenerationData()
	{
		delete[] perlinNoise;
		delete[] noiseSeed;
		delete[] fallOffMapA;
		delete[] fallOffMapB;
	}
};

class WorldData
{
private:
	const uint8_t MAP_WIDTH = 255;
	const uint8_t MAP_HEIGHT = 255;

	uint8_t falloffIndex_x;
	uint8_t falloffIndex_y;
	uint32_t generationIndex;
	WorldGenerationData* worldGenData = nullptr;

	const float NOISE_SCALE = 10.0f;

	// 2D noise variables
	uint16_t mapLength = 0;

	int nOctaveCount = 6;
	float fScalingBias = 1.3f;

	bool worldGenerated = false;
	uint8_t generatingWorld = 255;
	uint8_t generatingFalloff = 255;

	void PerlinNoise2D(int nWidth, int nHeight, int nOctaves, float fBias, float* noiseSeed, float* output);
	float Evaluate(float value, float falloffPoint = 1.75f);
	void ReseedNoise(float* noiseSeed);
	void ApplySeed(std::string seedString);

public:
	WorldData() = default;
	~WorldData();

public:
	uint8_t* tileData = nullptr;
	uint8_t* foliageData = nullptr;

	void GenerateMap();
	bool GenerateMapAsync();
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