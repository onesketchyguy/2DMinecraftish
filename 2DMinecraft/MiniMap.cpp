#include "Headers/miniMap.h"

void MiniMap::UpdateDecal(olc::vf2d localPlayer)
{
	// Called once per frame, draws random coloured pixels
	for (int x = 0; x < worldData->GetMapWidth(); x++)
		for (int y = 0; y < worldData->GetMapHeight(); y++)
		{
			float distY = abs((y * SPRITE_SCALE) - (localPlayer.y));
			float distX = abs((x * SPRITE_SCALE) - (localPlayer.x));

			if (distY < PLAYER_ICON_SCALE && distX < PLAYER_ICON_SCALE)
			{
				miniMapSprite->SetPixel(x, y, olc::Pixel(255, 0, 0, 255));
			}
			else
			{
				int dat = worldData->tileData[y * worldData->GetMapWidth() + x];

				miniMapSprite->SetPixel(x, y, mapColors[dat]);
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
		// Change location
		if (engine->GetKey(MOVE_MAP_KEY).bReleased)
		{
			miniMapDrawLocation++;
			miniMapDrawLocation %= 4;
		}

		// Scale in
		if (engine->GetKey(SCALE_IN_KEY).bHeld)
		{
			miniMapDrawScale += 0.01f;
			if (miniMapDrawScale > 1) miniMapDrawScale = 1;
		}

		// Scale out
		if (engine->GetKey(SCALE_OUT_KEY).bHeld)
		{
			miniMapDrawScale -= 0.01f;
			if (miniMapDrawScale < 0.05f) miniMapDrawScale = 0.05f;
		}

		// Only update the minimap every 'n'th frame
		if (time->frameCount % 20 == 0)
			UpdateDecal(localPlayer);

		// Draw the minimap
		olc::vf2d drawLocation;
		switch (miniMapDrawLocation)
		{
		case 0:
			drawLocation = { 0,0 };
			break;
		case 1:
			drawLocation = { engine->ScreenWidth() - miniMapSprite->width * miniMapDrawScale, 0 };
			break;
		case 2:
			drawLocation = { engine->ScreenWidth() - miniMapSprite->width * miniMapDrawScale, engine->ScreenHeight() - miniMapSprite->height * miniMapDrawScale };
			break;
		case 3:
			drawLocation = { 0, engine->ScreenHeight() - miniMapSprite->height * miniMapDrawScale };
			break;
		default:
			break;
		}

		engine->DrawDecal(drawLocation, miniMapDecal, { miniMapDrawScale, miniMapDrawScale });
	}
}