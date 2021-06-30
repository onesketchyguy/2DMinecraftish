#include "../Headers/miniMap.h"

float Evaluate(float value, float falloffPoint = 1.75f)
{
	if (value <= 0.001f) return 0;

	float a = 3;

	return pow(value, a) / (pow(value, a) + pow(falloffPoint - falloffPoint * value, a));
}

// FIXME: update this every time the map is modified
// FIXME: Optimize this code please
void MiniMap::UpdateSprite()
{
	// Clear the existing map
	for (int y = 0; y < worldData->GetMapHeight(); y++)
		for (int x = 0; x < worldData->GetMapWidth(); x++)
		{
			cachedMap->SetPixel(x, y, olc::BLANK);
		}

	// Generate a new map
	for (int y = 0; y < worldData->GetMapHeight() - 1; y++)
		for (int x = 1; x < worldData->GetMapWidth() - 1; x++)
		{
			if (x % 2 == 1 || y % 2 == 1) continue;

			float evalX = abs(x / (float)worldData->GetMapWidth() * 2.0f - 1.0f);
			float evalY = abs(y / (float)worldData->GetMapHeight() * 2.0f - 1.0f);
			float value = evalX > evalY ? evalX : evalY;
			float evaluation = Evaluate(value);

			int index = worldData->tileData[y * worldData->GetMapWidth() + x];
			olc::Pixel targetPixel = mapColors[index];

			evaluation *= 10;
			evaluation = ceil(evaluation);
			evaluation /= 10;

			if (evaluation > 0.7) evaluation += 2.5f;
			if (evaluation > 0.8) evaluation += 2.5f;
			if (evaluation > 0.9) evaluation += 2.5f;

			float factor = ceil(3.5f * evaluation);

			float newR = round(factor * targetPixel.r / 255.0f) * floor(255.0f / factor);
			float newG = round(factor * targetPixel.g / 255.0f) * floor(255.0f / factor);
			float newB = round(factor * targetPixel.b / 255.0f) * floor(255.0f / factor);
			float newA = round(factor * targetPixel.a / 255.0f) * floor(255.0f / factor);

			float errR = targetPixel.r - newR;
			float errG = targetPixel.g - newG;
			float errB = targetPixel.b - newB;
			float errA = targetPixel.a - newA;

			index = worldData->tileData[y * worldData->GetMapWidth() + (x + 1)];
			olc::Pixel col = mapColors[index];
			col.r = col.r + errR * 7.0f / 16.0f;
			col.g = col.g + errG * 7.0f / 16.0f;
			col.b = col.b + errB * 7.0f / 16.0f;
			col.a = col.a + errA * 7.0f / 16.0f;
			cachedMap->SetPixel(x + 1, y, col);

			index = worldData->tileData[(y + 1) * worldData->GetMapWidth() + (x - 1)];
			col = mapColors[index];
			col.r = col.r + errR * 3.0f / 16.0f;
			col.g = col.g + errG * 3.0f / 16.0f;
			col.b = col.b + errB * 3.0f / 16.0f;
			col.a = col.a + errA * 3.0f / 16.0f;
			cachedMap->SetPixel(x - 1, y + 1, col);

			index = worldData->tileData[(y + 1) * worldData->GetMapWidth() + x];
			col = mapColors[index];
			col.r = col.r + errR * 5.0f / 16.0f;
			col.g = col.g + errG * 5.0f / 16.0f;
			col.b = col.b + errB * 5.0f / 16.0f;
			col.a = col.a + errA * 5.0f / 16.0f;
			cachedMap->SetPixel(x, y + 1, col);

			index = worldData->tileData[y * worldData->GetMapWidth() + x];
			col = mapColors[index];
			col.r = col.r + errR * 1.0f / 16.0f;
			col.g = col.g + errG * 1.0f / 16.0f;
			col.b = col.b + errB * 1.0f / 16.0f;
			col.a = col.a + errA * 1.0f / 16.0f;
			cachedMap->SetPixel(x, y, col);
		}
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
	miniMapDrawScale = 0.825f;

	cachedMap = new olc::Sprite(worldData->GetMapWidth(), worldData->GetMapHeight());
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
			UpdateSprite();
			UpdateDecal(localPlayer);
		}
	}

	// Draw ye bloody minimap
	if (drawMiniMap == true)
	{
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