const uint8_t TILE_COUNT = 160; // The constant for how many MAX tiles can be drawn at once

const uint8_t MAP_WIDTH = 255;
const uint8_t MAP_HEIGHT = 255;

struct Tile {
public:
	uint8_t ID;
	int16_t x, y;
};

struct WorldData
{
private:
	const float NOISE_SCALE = 11.0f;

	byte* worldData;
	uint16_t mapLength = 0;

	// 2D noise variables
	int nOutputWidth = 100;
	int nOutputHeight = 100;
	float* fNoiseSeed2D = nullptr;
	float* fPerlinNoise2D = nullptr;

	// 1D noise variables
	float* fNoiseSeed1D = nullptr;
	float* fPerlinNoise1D = nullptr;
	int nOutputSize = 256;

	int nOctaveCount = 5;
	float fScalingBias = 1.6f;

	void PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutput)
	{
		// Used 1D Perlin Noise
		for (int x = 0; x < nCount; x++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nCount >> o;
				int nSample1 = (x / nPitch) * nPitch;
				int nSample2 = (nSample1 + nPitch) % nCount;

				float fBlend = (float)(x - nSample1) / (float)nPitch;

				float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];

				fScaleAcc += fScale;
				fNoise += fSample * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[x] = fNoise / fScaleAcc;
		}
	}

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

		nOutputSize = MAP_WIDTH;
		fNoiseSeed1D = new float[nOutputSize];
		fPerlinNoise1D = new float[nOutputSize];
		for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
	}

public:
	WorldData()
	{
		mapLength = MAP_WIDTH * MAP_HEIGHT;
		worldData = new byte[mapLength];

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

		for (uint16_t i = 0; i < mapLength; i++)
		{
			float x = i % MAP_WIDTH;
			float y = i / MAP_WIDTH;

			// Generate the noise
			int noiseIndex = y * nOutputWidth + x;

			// As the world reaches the edge we should blend it into the water
			int fallOffIndex = y * MAP_WIDTH + x;

			float perlinValue = fPerlinNoise2D[noiseIndex] - fallOffMap[fallOffIndex];
			//float perlinValue = fallOffMap[fallOffIndex];
			if (perlinValue < 0) perlinValue = 0;
			int pixel_bw = (int)(perlinValue * NOISE_SCALE);

			worldData[i] = pixel_bw;
		}

		tiles = new Tile[TILE_COUNT];
		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			tiles[i].x = i / 10;
			tiles[i].y = i % 10;
		}
	}

public:

	Tile* tiles = nullptr;

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

		return worldData[index];
	}

	olc::vf2d GetRandomGroundTile()
	{
		while (true)
		{
			int index = rand() % mapLength;
			if (worldData[index] > 1)
			{
				float x = index % MAP_WIDTH;
				float y = index / MAP_WIDTH;

				return { x * SPRITE_SCALE, y * SPRITE_SCALE };
			}
		}
	}

	Tile GetTile(float x, float y)
	{
		// This should find the best tile in range.
		// This is slow and I hate it but fuck it it works.

		uint8_t index = 0;

		float range = SPRITE_SCALE * 2;

		float bestX = range;
		float bestY = range;

		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			float distX = x - (tiles[i].x * (SPRITE_SCALE * 1.5f));
			float distY = y - (tiles[i].y * (SPRITE_SCALE * 1.5f));

			if ((distX > bestX - 0.1f && distX < bestX + 0.1f) && (distY > bestY - 0.1f && distY < bestY + 0.1f))
			{
				bestX = tiles[i].x;
				bestY = tiles[i].y;

				index = i;
			}
		}

		return tiles[index];
	}

	Tile* GetTilePtr(float x, float y)
	{
		// This should find the best tile in range.
		// This is slow and I hate it but fuck it it works.

		uint8_t index = 0;

		float range = SPRITE_SCALE * 2;

		float bestX = range;
		float bestY = range;

		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			float distX = x - (tiles[i].x * (SPRITE_SCALE * 1.5f));
			float distY = y - (tiles[i].y * (SPRITE_SCALE * 1.5f));

			if ((distX > bestX - 0.1f && distX < bestX + 0.1f) && (distY > bestY - 0.1f && distY < bestY + 0.1f))
			{
				bestX = tiles[i].x;
				bestY = tiles[i].y;

				index = i;
			}
		}

		return &tiles[index];
	}

	void MoveTiles(int screenWidth, int screenHeight, olc::vf2d camPosition)
	{
		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			tiles[i].x = (i / 10) + ceil(camPosition.x / SPRITE_SCALE);
			tiles[i].y = (i % 10) + ceil(camPosition.y / SPRITE_SCALE);

			tiles[i].x -= 5;
			tiles[i].y -= 1;

			tiles[i].ID = GetTileID(tiles[i].x, tiles[i].y);
		}
	}
};