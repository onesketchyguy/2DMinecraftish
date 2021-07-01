#include "../Headers/worldData.h"
#include "../Headers/bonusMaths.h";

WorldData::~WorldData()
{
	if (tileData != nullptr)
		delete[] tileData;
	if (foliageData != nullptr)
		delete[] foliageData;
}

void WorldData::PerlinNoise2D(int nWidth, int nHeight, int nOctaves, float fBias, float* noiseSeed, float* output)
{
	for (int x = 0; x < nWidth; x++)
		for (int y = 0; y < nHeight; y++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nWidth >> o;
				int nSampleX1 = (x / nPitch) * nPitch;
				int nSampleY1 = (y / nPitch) * nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
				int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

				float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
				float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

				float fSampleT = (1.0f - fBlendX) * noiseSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * noiseSeed[nSampleY1 * nWidth + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * noiseSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * noiseSeed[nSampleY2 * nWidth + nSampleX2];

				fScaleAcc += fScale;
				fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			output[y * nWidth + x] = fNoise / fScaleAcc;
		}
}

void WorldData::ReseedNoise(float* noiseSeed)
{
	for (int i = 0; i < mapLength; i++)
	{
		noiseSeed[i] = (float)rand() / (float)RAND_MAX;
	}
}

void WorldData::ApplySeed(std::string seedString)
{
	unsigned int value = 0;
	for (size_t i = 0; i < seedString.length(); i++)
		value = (value + static_cast<int>(seedString[i])) % INT32_MAX;

	srand(value);
}

void WorldData::GenerateCollisionMap()
{
	for (uint16_t i = 0; i < mapLength; i++)
	{
		collisionData[i] = false;

		if (foliageData[i] == 1 || foliageData[i] == 2)
		{
			collisionData[i] = true;
		}

		if (tileData[i] == 0 || tileData[i] == 1)
		{
			collisionData[i] = true;
		}
	}
}

void WorldData::ClearMap()
{
	if (tileData != nullptr)
		delete[] tileData;
	if (foliageData != nullptr)
		delete[] foliageData;

	worldGenerated = false;
	generating = false;
	generatingWorld = 255;
	generatingFalloff = 255;
}

void WorldData::GenerateMap()
{
	generating = true;

	mapLength = MAP_WIDTH * MAP_HEIGHT;

	tileData = new uint8_t[mapLength];
	foliageData = new uint8_t[mapLength];
	collisionData = new bool[mapLength];
	worldGenData = new WorldGenerationData(mapLength);
	ReseedNoise(worldGenData->noiseSeed);

	print("Generating map...");

	worldGenerated = false;
	generatingWorld = 0;

	print("Generating falloff map...");

	// Seed the noise
	PerlinNoise2D(MAP_WIDTH, MAP_HEIGHT, nOctaveCount, fScalingBias, worldGenData->noiseSeed, worldGenData->perlinNoise);

	generatingFalloff = 0;

	// Feed the noise
	// Generate falloff map
	for (int mapX = 0; mapX < MAP_WIDTH; mapX++)
	{
		for (int mapY = 0; mapY < MAP_HEIGHT; mapY++)
		{
			float evalX = abs(mapX / (float)MAP_WIDTH * 2 - 1);
			float evalY = abs(mapY / (float)MAP_HEIGHT * 2 - 1);

			float value = evalX > evalY ? evalX : evalY;

			// Generate the noise
			int noiseIndex = mapY * MAP_WIDTH + mapX;
			float perlinValue = (worldGenData->perlinNoise[noiseIndex] + value) / 2.0f;

			worldGenData->fallOffMapA[mapY * MAP_WIDTH + mapX] = Evaluate(perlinValue);
			worldGenData->fallOffMapB[mapY * MAP_WIDTH + mapX] = Evaluate(mapX, mapY, MAP_WIDTH, MAP_HEIGHT);//Evaluate(value);

			generatingFalloff = static_cast<int>(mapX + mapY / static_cast<float>(mapLength));
		}
	}

	print("Generated falloff map.");

	ReseedNoise(worldGenData->noiseSeed);
	PerlinNoise2D(MAP_WIDTH, MAP_HEIGHT, nOctaveCount, fScalingBias, worldGenData->noiseSeed, worldGenData->perlinNoise);

	for (int i = 0; i < mapLength; i++)
	{
		float x = static_cast<float>(i % MAP_WIDTH);
		float y = static_cast<float>(i / MAP_WIDTH);

		if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) tileData[i] = 0;
		else
		{
			// As the world reaches the edge we should blend it into the water
			float fallOff = worldGenData->fallOffMapA[i] + worldGenData->fallOffMapB[i];

			// Generate the noise
			float perlinValue = worldGenData->perlinNoise[i] - fallOff;
			if (perlinValue < 0) perlinValue = 0;
			int pixel_bw = static_cast<int>(perlinValue * NOISE_SCALE);

			tileData[i] = pixel_bw;

			// Place some foliage
			bool shouldPlace = rand() % 100 > 89; // 10% chance of spanwing foliage

			if (shouldPlace) {
				switch (tileData[i])
				{
				case 3: // Dead earth layer
					foliageData[i] = 1;
					break;
				case 4: // grass layer
					foliageData[i] = rand() % 5;
					break;
				default: // Who fucking knows
					foliageData[i] = 0;
					break;
				}
			}
			else
			{
				foliageData[i] = 0;
			}

			// Set the progress bar
			generatingWorld = static_cast<int>(i / static_cast<float>(mapLength)) * 255;
		}
	}

	delete worldGenData;

	auto groundTile = GetRandomGroundTile();

	if (groundTile.x == 0 && groundTile.y == 0) worldGenerated = false;
	else worldGenerated = true;

	generatingWorld = 255;

	if (worldGenerated)
	{
		print("Generated map.");
		GenerateCollisionMap();
	}
	else
	{
		print("Failed to generate map.");
	}

	generating = false;
}

bool WorldData::GenerateMapAsync()
{
	if (worldGenData == nullptr)
	{
		mapLength = MAP_WIDTH * MAP_HEIGHT;

		worldGenData = new WorldGenerationData(mapLength);

		ReseedNoise(worldGenData->noiseSeed);

		print("Generating map aync...");

		worldGenerated = false;
		generatingWorld = 0;

		print("Generating falloff map async...");

		// Seed the noise
		PerlinNoise2D(MAP_WIDTH, MAP_HEIGHT, nOctaveCount, fScalingBias, worldGenData->noiseSeed, worldGenData->perlinNoise);

		generatingFalloff = 0;
		falloffIndex_x = 0;
		falloffIndex_y = 0;
		generationIndex = 0;
	}

	// Feed the noise
	// Generate falloff map
	if (falloffIndex_x < MAP_WIDTH)
	{
		float evalX = abs(falloffIndex_x / (float)MAP_WIDTH * 2 - 1);
		float evalY = abs(falloffIndex_y / (float)MAP_HEIGHT * 2 - 1);

		// Generate the noise
		int noiseIndex = falloffIndex_y * MAP_WIDTH + falloffIndex_x;
		float perlinValue = worldGenData->perlinNoise[noiseIndex];

		float value = evalX > evalY ? evalX : evalY;
		worldGenData->fallOffMapA[falloffIndex_y * MAP_WIDTH + falloffIndex_x] = Evaluate(perlinValue);
		worldGenData->fallOffMapB[falloffIndex_y * MAP_WIDTH + falloffIndex_x] = Evaluate(value);

		generatingFalloff = static_cast<int>(falloffIndex_x + falloffIndex_y / static_cast<float>(mapLength));

		falloffIndex_y++;
		if (falloffIndex_y < MAP_HEIGHT)
		{
			falloffIndex_x++;
			falloffIndex_y = 0;
		}

		return false;
	}

	if (tileData == nullptr && foliageData == nullptr)
	{
		print("Generated falloff map.");

		tileData = new uint8_t[mapLength];
		foliageData = new uint8_t[mapLength];
	}
	else
	{
		if (worldGenData->worldSeeded == false)
		{
			ReseedNoise(worldGenData->noiseSeed);
			PerlinNoise2D(MAP_WIDTH, MAP_HEIGHT, nOctaveCount, fScalingBias, worldGenData->noiseSeed, worldGenData->perlinNoise);

			worldGenData->worldSeeded = true;
		}

		if (generationIndex < mapLength)
		{
			int i = generationIndex;

			float x = static_cast<float>(i % MAP_WIDTH);
			float y = static_cast<float>(i / MAP_WIDTH);

			if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) tileData[i] = 0;
			else
			{
				// As the world reaches the edge we should blend it into the water
				float fallOff = worldGenData->fallOffMapA[i] + worldGenData->fallOffMapB[i];

				// Generate the noise
				float perlinValue = worldGenData->perlinNoise[i] - fallOff;
				if (perlinValue < 0) perlinValue = 0;
				int pixel_bw = static_cast<int>(perlinValue * NOISE_SCALE);

				tileData[i] = pixel_bw;

				// Place some foliage
				bool shouldPlace = rand() % 100 > 89; // 10% chance of spanwing foliage

				if (shouldPlace) {
					switch (tileData[i])
					{
					case 3: // Dead earth layer
						foliageData[i] = 1;
						break;
					case 4: // grass layer
						foliageData[i] = rand() % 5;
						break;
					default: // Who fucking knows
						foliageData[i] = 0;
						break;
					}
				}
				else
				{
					foliageData[i] = 0;
				}

				// Set the progress bar
				generatingWorld = static_cast<int>(i / static_cast<float>(mapLength)) * 255;
			}

			generationIndex++;

			return false;
		}

		delete worldGenData;

		auto groundTile = GetRandomGroundTile();

		if (groundTile.x == 0 && groundTile.y == 0) worldGenerated = false;
		else worldGenerated = true;

		generatingWorld = 255;

		print("Generated map.");
	}

	return true;
}

bool WorldData::GetWorldGenerating()
{
	return generating;
}

bool WorldData::GetWorldGenerated()
{
	return worldGenerated;
}

float WorldData::GetWorldProgress()
{
	return generatingWorld / 255.0f;
}

float WorldData::GetFalloffProgress()
{
	return generatingFalloff / 255.0f;
}

int WorldData::GetMapWidth()
{
	return MAP_WIDTH;
}

int WorldData::GetMapHeight()
{
	return MAP_HEIGHT;
}

int WorldData::GetTileID(int x, int y)
{
	if (x < 0) {
		x *= -1;
		x %= MAP_WIDTH;
		x = MAP_WIDTH - x;
	}

	if (y < 0) {
		y *= -1;
		y %= MAP_WIDTH;
		y = MAP_HEIGHT - y;
	}

	if (x > MAP_WIDTH) {
		x %= MAP_WIDTH;
	}

	if (y > MAP_HEIGHT) {
		y %= MAP_HEIGHT;
	}

	int index = y * MAP_WIDTH + x;

	return tileData[index];
}

int WorldData::GetTileID(float x, float y)
{
	float xPos = ceil(x / SPRITE_SCALE);
	float yPos = floor(y / SPRITE_SCALE);
	return GetTileID(int(xPos), int(yPos));
}

int WorldData::GetFoliageID(int x, int y)
{
	if (x < 0) {
		x *= -1;
		x %= MAP_WIDTH;
		x = MAP_WIDTH - x;
	}

	if (y < 0) {
		y *= -1;
		y %= MAP_WIDTH;
		y = MAP_HEIGHT - y;
	}

	if (x > MAP_WIDTH) {
		x %= MAP_WIDTH;
	}

	if (y > MAP_HEIGHT) {
		y %= MAP_HEIGHT;
	}

	int index = y * MAP_WIDTH + x;

	return foliageData[index];
}

int WorldData::GetFoliageID(float x, float y)
{
	float xPos = ceil(x / SPRITE_SCALE);
	float yPos = floor(y / SPRITE_SCALE);
	return GetFoliageID(int(xPos), int(yPos));
}

int WorldData::GetFoliageIndex(float fx, float fy)
{
	int x = int(ceil(fx / SPRITE_SCALE));
	int y = int(floor(fy / SPRITE_SCALE));

	if (x < 0) {
		x *= -1;
		x %= MAP_WIDTH;
		x = MAP_WIDTH - x;
	}

	if (y < 0) {
		y *= -1;
		y %= MAP_WIDTH;
		y = MAP_HEIGHT - y;
	}

	if (x > MAP_WIDTH) {
		x %= MAP_WIDTH;
	}

	if (y > MAP_HEIGHT) {
		y %= MAP_HEIGHT;
	}

	return y * MAP_WIDTH + x;
}

void WorldData::SetTileID(float x, float y, uint8_t value)
{
	int xPos = int(ceil(x / SPRITE_SCALE));
	int yPos = int(floor(y / SPRITE_SCALE));

	if (xPos < 0) {
		xPos *= -1;
		xPos %= MAP_WIDTH;
		xPos = static_cast<int>(MAP_WIDTH - x);
	}

	if (yPos < 0) {
		yPos *= -1;
		yPos %= MAP_WIDTH;
		yPos = static_cast<int>(MAP_HEIGHT - y);
	}

	if (xPos > MAP_WIDTH) {
		xPos %= MAP_WIDTH;
	}

	if (yPos > MAP_HEIGHT) {
		yPos %= MAP_HEIGHT;
	}

	int index = yPos * MAP_WIDTH + xPos;
	tileData[index] = value;
	std::cout << "Changing tile index from: " << xPos << "," << yPos << std::endl;
}

void WorldData::SetFoliageID(float x, float y, uint8_t value)
{
	int xPos = int(ceil(x / SPRITE_SCALE));
	int yPos = int(floor(y / SPRITE_SCALE));

	if (xPos < 0) {
		xPos *= -1;
		xPos %= MAP_WIDTH;
		xPos = static_cast<int>(MAP_WIDTH - x);
	}

	if (yPos < 0) {
		yPos *= -1;
		yPos %= MAP_WIDTH;
		yPos = static_cast<int>(MAP_HEIGHT - y);
	}

	if (xPos > MAP_WIDTH) {
		xPos %= MAP_WIDTH;
	}

	if (yPos > MAP_HEIGHT) {
		yPos %= MAP_HEIGHT;
	}

	int index = yPos * MAP_WIDTH + xPos;
	foliageData[index] = value;
}

olc::vf2d WorldData::GetRandomGroundTile()
{
	int attempts = 0;

	while (true)
	{
		int index = rand() % mapLength;
		if (tileData[index] > 1)
		{
			float x = static_cast<float>(index % MAP_WIDTH);
			float y = index / static_cast<float>(MAP_WIDTH);

			return { x * SPRITE_SCALE, y * SPRITE_SCALE };
		}
		attempts++;

		if (attempts > 100) {
			print("Unable to get spawn point.");
			return { 0.0f, 0.0f };
		}
	}
}