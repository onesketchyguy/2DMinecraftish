#include "../Headers/miniMap.h"

olc::Pixel MiniMap::GetDitherColor(int x, int y, float mutator, double4 error)
{
	int index = worldData->tileData[y * worldData->GetMapWidth() + x];
	olc::Pixel col = mapColors[index];

	col.r = col.r + static_cast<uint8_t>(round(error.x * mutator / 16.0f));
	col.g = col.g + static_cast<uint8_t>(round(error.y * mutator / 16.0f));
	col.b = col.b + static_cast<uint8_t>(round(error.z * mutator / 16.0f));
	col.a = col.a + static_cast<uint8_t>(round(error.w * mutator / 16.0f));
	return col;
}

// FIXME: Optimize this code please
void MiniMap::UpdateSprite()
{
	olc::Pixel targetPixel;
	double4 newCol;
	double4 error;

	// Generate a new map
	for (int y = 0; y < worldData->GetMapHeight() - 1; y += 2)
		for (int x = 1; x < worldData->GetMapWidth() - 1; x += 2)
		{
			//if (x % 2 == 1 || y % 2 == 1) continue; // If not doing this then increment 2 instead of 1 in for loops

			float factor = Evaluate(x, y, worldData->GetMapWidth(), worldData->GetMapHeight());

			int index = worldData->tileData[y * worldData->GetMapWidth() + x];
			targetPixel = mapColors[index];

			if (factor > 0.5)
			{
				factor += factor * 2.5f;
			}

			if (factor > 4.0f) factor = 4.0f;
			if (factor < 1.0f) factor = 1.0f;

			newCol = double4(static_cast<double>(targetPixel.r), static_cast<double>(targetPixel.g),
				static_cast<double>(targetPixel.b), static_cast<double>(targetPixel.a));
			newCol.x = round(factor * static_cast<double>(targetPixel.r) / 255.0f) * floor(255.0f / factor);
			newCol.y = round(factor * static_cast<double>(targetPixel.g) / 255.0f) * floor(255.0f / factor);
			newCol.z = round(factor * static_cast<double>(targetPixel.b) / 255.0f) * floor(255.0f / factor);
			newCol.w = round(factor * static_cast<double>(targetPixel.a) / 255.0f) * floor(255.0f / factor);

			error = double4(static_cast<double>(targetPixel.r - newCol.x), static_cast<double>(targetPixel.g - newCol.y),
				static_cast<double>(targetPixel.b - newCol.z), static_cast<double>(targetPixel.a - newCol.w));

			cachedMap->SetPixel(x + 1, y, GetDitherColor(x + 1, y, 7.0f, error));
			cachedMap->SetPixel(x - 1, y + 1, GetDitherColor(x - 1, y + 1, 3.0f, error));
			cachedMap->SetPixel(x, y + 1, GetDitherColor(x - 1, y + 1, 5.0f, error));
			cachedMap->SetPixel(x, y, GetDitherColor(x - 1, y + 1, 1.0f, error));
		}

	/*
	int evaluateSide = 5;
	for (size_t x = 0; x <= evaluateSide; x++)
	{
		//auto value = Evaluate(x, x, evaluateSide, evaluateSide);
		//print("Evaluated " + std::to_string(x) + ", " + std::to_string(x) + ": " + std::to_string(value));

		for (size_t y = 0; y <= evaluateSide; y++)
		{
			auto value = Evaluate(x, y, evaluateSide, evaluateSide);
			print("Evaluated " + std::to_string(x) + ", " + std::to_string(y) + ": " + std::to_string(value));
		}
	}
	*/
}

void MiniMap::UpdateDecal(olc::vf2d localPlayer)
{
	// Called once per frame, draws random coloured pixels
	for (int y = 0; y < worldData->GetMapHeight(); y++)
		for (int x = 0; x < worldData->GetMapWidth(); x++)
		{
			float distY = abs((y * SPRITE_SCALE) - (localPlayer.y));
			float distX = abs((x * SPRITE_SCALE) - (localPlayer.x));

			if (distY < PLAYER_ICON_SCALE && distX < PLAYER_ICON_SCALE)
			{
				miniMapSprite->SetPixel(x, y, playerColor);
			}
			else
			{
				if (distY < CLEAN_AREA_SCALE && distX < CLEAN_AREA_SCALE)
				{
					int index = worldData->tileData[y * worldData->GetMapWidth() + x];
					olc::Pixel targetPixel = mapColors[index];

					miniMapSprite->SetPixel(x, y, targetPixel);
				}
				else
				{
					miniMapSprite->SetPixel(x, y, cachedMap->GetPixel(x, y));
				}
			}
		}

	miniMapDecal->Update();
}

void MiniMap::Initialize(olc::Sprite* tileSprite, WorldData* worldData,
	olc::PixelGameEngine* engine, TimeConstruct* time)
{
	this->worldData = worldData;
	this->engine = engine;
	this->time = time;

	// This is just a nice scale, we can really use any value here.
	miniMapDrawScale = 0.625f;

	cachedMap = new olc::Sprite(worldData->GetMapWidth(), worldData->GetMapHeight());
	for (int y = 0; y < worldData->GetMapHeight(); y++)
		for (int x = 0; x < worldData->GetMapWidth(); x++)
			cachedMap->SetPixel(x, y, olc::BLANK);

	miniMapSprite = new olc::Sprite(worldData->GetMapWidth(), worldData->GetMapHeight());
	miniMapDecal = new olc::Decal(miniMapSprite);

	uint8_t length = WORLD_TILES_WIDTH * WORLD_TILES_HEIGHT;

	mapColors = new olc::Pixel[length];

	for (uint8_t i = 0; i < length; i++)
	{
		int x = i % WORLD_TILES_WIDTH;
		int y = i / WORLD_TILES_WIDTH;

		x *= SPRITE_SCALE;
		y *= SPRITE_SCALE;

		mapColors[i] = tileSprite->GetPixel(x, y);
	}
}

void MiniMap::UpdateMiniMap(olc::vf2d localPlayer)
{
	if (engine->GetKey(TOGGLE_KEY).bReleased)
	{
		drawMiniMap = !drawMiniMap;

		if (drawMiniMap == true)
		{
			UpdateDecal(localPlayer);
		}
	}

	// Draw ye bloody minimap
	if (drawMiniMap == true)
	{
		// FIXME: update this every time the map is modified
		if (updateMap == true)
		{
			UpdateSprite();
			updateMap = false;
		}

		// Only update the minimap every 'n'th frame
		if (time->frameCount % 20 == 0) UpdateDecal(localPlayer);

		// Draw the minimap
		olc::vf2d drawLocation =
		{
			(engine->ScreenWidth() * 0.5f) - (miniMapSprite->width * miniMapDrawScale * 0.5f),
			(engine->ScreenHeight() * 0.5f) - (miniMapSprite->height * miniMapDrawScale * 0.5f)
		};

		engine->SetPixelMode(olc::Pixel::Mode::ALPHA);

		engine->DrawDecal(drawLocation, miniMapDecal, { miniMapDrawScale,  miniMapDrawScale });

		engine->SetPixelMode(olc::Pixel::Mode::NORMAL);
	}
}