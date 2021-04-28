#pragma once
struct WorldData
{
private:
	byte* worldData;

	const uint8_t MAP_WIDTH = 255;
	const uint8_t MAP_HEIGHT = 255;

public:
	WorldData()
	{
		tiles = new Tile[TILE_COUNT];
		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			tiles[i].x = i / 10;
			tiles[i].y = i % 10;

			//worldTiles[i].ID = rand() % 2;
			tiles[i].ID = 0;
		}

		worldData = new byte[MAP_WIDTH * MAP_HEIGHT];
	}

public:

	Tile* tiles = nullptr;

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
			float distX = tiles[i].x - x;
			float distY = tiles[i].y - y;

			if ((distX > bestX - 0.1f && distX < bestX + 0.1f) && (distY > bestY - 0.1f && distY < bestY + 0.1f))
			{
				bestX = tiles[i].x;
				bestY = tiles[i].y;

				index = i;
			}
		}

		return tiles[index];
	}

	void MoveTiles(int screenWidth, int screenHeight, olc::vf2d camPosition)
	{
		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			tiles[i].x = (i / 10) + ceil(camPosition.x / SPRITE_SCALE);
			tiles[i].y = (i % 10) + ceil(camPosition.y / SPRITE_SCALE);

			tiles[i].x -= 5;
			tiles[i].y -= 1;
		}
	}
};