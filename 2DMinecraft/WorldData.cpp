#include "Headers/worldData.h"

void WorldData::PerlinNoise2D(int nWidth, int nHeight, int nOctaves, float fBias, float* output)
{
	// Used 1D Perlin Noise
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

float WorldData::Evaluate(float value, float falloffPoint)
{
	if (value <= 0.001f) return 0;

	float a = 3;

	return pow(value, a) / (pow(value, a) + pow(falloffPoint - falloffPoint * value, a));
}

void WorldData::ReseedNoise()
{
	if (noiseSeed != nullptr)
	{
		delete[] noiseSeed;
	}

	noiseSeed = new float[outputLength];
	for (int i = 0; i < outputLength; i++)
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

void WorldData::InitNoise()
{
	outputLength = MAP_WIDTH * MAP_HEIGHT;
	perlinNoise = new float[outputLength];
	ReseedNoise();
}

void WorldData::GenerateMap()
{
	tileData = new uint8_t[mapLength];
	foliageData = new uint8_t[mapLength];
	float* fallOffMapA = new float[mapLength];
	float* fallOffMapB = new float[mapLength];

	InitNoise();

	print("Generating map...");

	worldGenerated = false;
	generatingWorld = 0;

	print("Generating falloff map...");

	// Seed the noise
	PerlinNoise2D(MAP_WIDTH, MAP_HEIGHT, nOctaveCount, fScalingBias, perlinNoise);

	generatingFalloff = 0;

	// Feed the noise
	// Generate falloff map
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			float _x = abs(x / (float)MAP_WIDTH * 2 - 1);
			float _y = abs(y / (float)MAP_HEIGHT * 2 - 1);

			// Generate the noise
			int noiseIndex = y * MAP_WIDTH + x;
			float perlinValue = perlinNoise[noiseIndex];

			float value = _x > _y ? _x : _y;
			fallOffMapA[y * MAP_WIDTH + x] = Evaluate(perlinValue);
			fallOffMapB[y * MAP_WIDTH + x] = Evaluate(value);

			generatingFalloff = static_cast<int>(x + y / static_cast<float>(mapLength));
		}
	}

	print("Generated falloff map.");

	ReseedNoise();
	PerlinNoise2D(MAP_WIDTH, MAP_HEIGHT, nOctaveCount, fScalingBias, perlinNoise);

	for (int i = 0; i < mapLength; i++)
	{
		float x = static_cast<float>(i % MAP_WIDTH);
		float y = static_cast<float>(i / MAP_WIDTH);

		if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) tileData[i] = 0;
		else
		{
			// As the world reaches the edge we should blend it into the water
			float fallOff = fallOffMapA[i] + fallOffMapB[i];

			// Generate the noise
			float perlinValue = perlinNoise[i] - fallOff;
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

	delete[] perlinNoise;
	delete[] noiseSeed;

	worldGenerated = true;
	generatingWorld = 255;

	print("Generated map.");
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
			return { -1.0f, 1.0f };
		}
	}
}