const uint8_t TILE_COUNT = 160; // The constant for how many MAX tiles can be drawn at once

const uint8_t MAP_WIDTH = 255;
const uint8_t MAP_HEIGHT = 255;
uint32_t mapLength = 0;

struct Tile {
public:
	uint8_t tileID;
	uint8_t foliageID;
	int16_t x, y;
};

struct WorldData
{
private:
	const float NOISE_SCALE = 10.0f;
	// 2D noise variables
	int nOutputWidth = 100;
	int nOutputHeight = 100;
	float* fNoiseSeed2D = nullptr;
	float* fPerlinNoise2D = nullptr;

	int nOctaveCount = 5;
	float fScalingBias = 1.6f;

	void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput)
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

					float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
					float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

					fScaleAcc += fScale;
					fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
					fScale = fScale / fBias;
				}

				// Scale to seed range
				fOutput[y * nWidth + x] = fNoise / fScaleAcc;
			}
	}

	float Evaluate(float value, float falloffPoint = 1.75f)
	{
		float a = 3;

		return pow(value, a) / (pow(value, a) + pow(falloffPoint - falloffPoint * value, a));
	}

	void ReseedNoise()
	{
		for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
		PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);
	}

	void InitNoise()
	{
		nOutputWidth = MAP_WIDTH;
		nOutputHeight = MAP_HEIGHT;

		fNoiseSeed2D = new float[nOutputWidth * nOutputHeight];
		fPerlinNoise2D = new float[nOutputWidth * nOutputHeight];
		for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
	}

public:
	WorldData()
	{
		mapLength = MAP_WIDTH * MAP_HEIGHT;
		tileData = new uint8_t[mapLength];
		foliageData = new uint8_t[mapLength];

		InitNoise();

		PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

		// Generate falloff map
		auto fallOffMap = new float[mapLength];
		for (size_t x = 0; x < MAP_WIDTH; x++)
		{
			for (size_t y = 0; y < MAP_HEIGHT; y++)
			{
				float _x = abs(x / (float)MAP_WIDTH * 2 - 1);
				float _y = abs(y / (float)MAP_HEIGHT * 2 - 1);

				// Generate the noise
				int noiseIndex = y * nOutputWidth + x;
				float perlinValue = fPerlinNoise2D[noiseIndex];

				float value = _x > _y ? _x : _y;
				fallOffMap[y * MAP_WIDTH + x] = Evaluate(value - perlinValue);
			}
		}

		ReseedNoise();

		for (uint64_t i = 0; i < mapLength; i++)
		{
			float x = i % MAP_WIDTH;
			float y = i / MAP_WIDTH;
			if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) tileData[i] = 0;
			else
			{
				// Generate the noise
				int noiseIndex = y * nOutputWidth + x;

				// As the world reaches the edge we should blend it into the water
				int fallOffIndex = y * MAP_WIDTH + x;

				float perlinValue = fPerlinNoise2D[noiseIndex] - fallOffMap[fallOffIndex];
				//float perlinValue = fallOffMap[fallOffIndex];
				if (perlinValue < 0) perlinValue = 0;
				int pixel_bw = (int)(perlinValue * NOISE_SCALE);

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
					}
				}
				else
				{
					foliageData[i] = 0;
				}
			}

			tiles = new Tile[TILE_COUNT];
			for (uint8_t i = 0; i < TILE_COUNT; i++)
			{
				tiles[i].x = i / 10;
				tiles[i].y = i % 10;
			}
		}
	}

public:
	Tile* tiles = nullptr;
	uint8_t* tileData = nullptr;
	uint8_t* foliageData = nullptr;

	int GetTileID(int x, int y)
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

	int GetTileID(float x, float y)
	{
		float xPos = ceil(x / SPRITE_SCALE);
		float yPos = floor(y / SPRITE_SCALE);
		return GetTileID(int(xPos), int(yPos));
	}

	int GetFoliageID(int x, int y)
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

	int GetFoliageID(float x, float y)
	{
		float xPos = ceil(x / SPRITE_SCALE);
		float yPos = floor(y / SPRITE_SCALE);
		return GetFoliageID(int(xPos), int(yPos));
	}

	int GetFoliageIndex(float fx, float fy)
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

	void SetTileID(float x, float y, uint8_t value)
	{
		int xPos = int(ceil(x / SPRITE_SCALE));
		int yPos = int(floor(y / SPRITE_SCALE));

		if (xPos < 0) {
			xPos *= -1;
			xPos %= MAP_WIDTH;
			xPos = MAP_WIDTH - x;
		}

		if (yPos < 0) {
			yPos *= -1;
			yPos %= MAP_WIDTH;
			yPos = MAP_HEIGHT - y;
		}

		if (xPos > MAP_WIDTH) {
			xPos %= MAP_WIDTH;
		}

		if (yPos > MAP_HEIGHT) {
			yPos %= MAP_HEIGHT;
		}

		int index = y * MAP_WIDTH + x;
		tileData[index] = value;
		std::cout << "Changing tile index from: " << xPos << "," << yPos << std::endl;
	}

	void SetFoliageID(float x, float y, uint8_t value)
	{
		int xPos = int(ceil(x / SPRITE_SCALE));
		int yPos = int(floor(y / SPRITE_SCALE));

		if (xPos < 0) {
			xPos *= -1;
			xPos %= MAP_WIDTH;
			xPos = MAP_WIDTH - x;
		}

		if (yPos < 0) {
			yPos *= -1;
			yPos %= MAP_WIDTH;
			yPos = MAP_HEIGHT - y;
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

	Tile GetTile(float x, float y)
	{
		int xPos = int(ceil(x / SPRITE_SCALE));
		int yPos = int(floor(y / SPRITE_SCALE));

		if (xPos < 0) {
			xPos *= -1;
			xPos %= MAP_WIDTH;
			xPos = MAP_WIDTH - x;
		}

		if (yPos < 0) {
			yPos *= -1;
			yPos %= MAP_WIDTH;
			yPos = MAP_HEIGHT - y;
		}

		if (xPos > MAP_WIDTH) {
			xPos %= MAP_WIDTH;
		}

		if (yPos > MAP_HEIGHT) {
			yPos %= MAP_HEIGHT;
		}

		int index = y * MAP_WIDTH + x;

		return tiles[index];
	}

	olc::vf2d GetRandomGroundTile()
	{
		while (true)
		{
			int index = rand() % mapLength;
			if (tileData[index] > 1)
			{
				float x = index % MAP_WIDTH;
				float y = index / MAP_WIDTH;

				return { x * SPRITE_SCALE, y * SPRITE_SCALE };
			}
		}
	}

	void MoveTiles(olc::vf2d camPosition)
	{
		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			tiles[i].x = (i / 10) + ceil(camPosition.x / SPRITE_SCALE);
			tiles[i].y = (i % 10) + ceil(camPosition.y / SPRITE_SCALE);

			tiles[i].x -= 5;
			tiles[i].y -= 1;

			tiles[i].tileID = GetTileID(tiles[i].x, tiles[i].y);
			tiles[i].foliageID = GetFoliageID(tiles[i].x, tiles[i].y);
		}
	}
};