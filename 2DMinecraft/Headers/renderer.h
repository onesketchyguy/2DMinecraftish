#pragma once

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

#define NEW_RENDERER

#ifdef NEW_RENDERER

class Renderer
{
private:
	olc::vf2d screenCenter;
	olc::vf2d camTarget;

	const float MIN_ZOOM = 6.0f;
	const float MAX_ZOOM = 36.0f;

public:
	Renderer(olc::PixelGameEngine* engine, WorldData* worldData)
	{
		this->engine = engine;
		this->worldData = worldData;

		// Render scale shit

		print("Generating sprites...");

		viewPort = olc::TileTransformedView({ engine->ScreenWidth(), engine->ScreenHeight() },
			{ SPRITE_SCALE, SPRITE_SCALE });

		print("Created viewport.");

		// Player shit
		playerSpriteData = new olc::Renderable();
		LoadSprites(playerSpriteData, "Data/player.png");

		// Item shit
		itemSpriteData = new olc::Renderable();
		LoadSprites(itemSpriteData, "Data/items.png");

		// World shit
		tileSpriteData = new olc::Renderable();
		LoadSprites(tileSpriteData, "Data/tiles.png");
	}

public:
	olc::PixelGameEngine* engine = nullptr;

	olc::TileTransformedView viewPort;

	WorldData* worldData = nullptr;
	olc::Renderable* tileSpriteData = nullptr;
	olc::Renderable* itemSpriteData = nullptr;
	olc::Renderable* playerSpriteData = nullptr;

	void LoadSprites(olc::Renderable* renderable, std::string dir)
	{
		renderable->Load(dir);
		print("Loading sprites from: " + dir);

		if (renderable->Decal() == nullptr)
		{
			print("Could not load sprites!");
			DEBUG = true;
			APPLICATION_RUNNING = true;
		}

		print("Loaded sprites.");
	}

	void DrawDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal, olc::Pixel color = { 255,255,255,255 })
	{
		viewPort.DrawDecal(pos, decal, scale, color);
	}

	void DrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal, olc::vi2d cell = { 0,0 }, olc::Pixel color = { 255,255,255,255 })
	{
		viewPort.DrawPartialDecal(pos, scale, decal, cell, spriteScale, color);
	}

	void DrawObject(Object* obj) {
		engine->SetPixelMode(olc::Pixel::NORMAL);

		olc::vi2d decalScale = {
			obj->GetLookDir() == ANIMATION::left ? -spriteScale.x : spriteScale.x, spriteScale.y };
		olc::vi2d spriteCell = { spriteScale.x * obj->GetCellIndex().x, spriteScale.y * obj->GetCellIndex().y };

		olc::vf2d offset = { obj->GetLookDir() == ANIMATION::left ? spriteScale.x : 0.0f , 0.0f };

		DrawPartialDecal(obj->GetPosition() + offset, decalScale,
			obj->GetRenderable()->Decal(), spriteCell);
	}

	void DrawPlayer(olc::vf2d pos) {
		engine->SetPixelMode(olc::Pixel::NORMAL);

		DrawPartialDecal(pos, spriteScale, playerSpriteData->Decal(), { 0, 0 });
	}

	void DrawTile(int mapIndex, float x, float y) {
		engine->SetPixelMode(olc::Pixel::NORMAL);

		olc::vf2d pos = { x, y };

		int foliage = worldData->foliageData[mapIndex];
		int tile = worldData->tileData[mapIndex];

		int cellIndex_x = tile % WORLD_TILES_WIDTH;
		int cellIndex_y = tile / WORLD_TILES_WIDTH;
		olc::vi2d tileSpriteCell = {
			spriteScale.x * cellIndex_x,
			spriteScale.y * cellIndex_y };

		// Draw tile
		DrawPartialDecal(pos, spriteScale, tileSpriteData->Decal(), tileSpriteCell);

		if (foliage > 0) // 0 = no foliage
		{
			foliage -= 1; // Subtract 1 to put this layer back into sprite space

			olc::vi2d foliageSpriteCell = { spriteScale.x * foliage, spriteScale.x * 2 };

			// Draw foliage
			DrawPartialDecal(pos, spriteScale, tileSpriteData->Decal(), tileSpriteCell);
		}
	}

	void DrawItem(Item obj) {
		engine->SetPixelMode(olc::Pixel::NORMAL);

		olc::vi2d spriteCell{ obj.ID % WORLD_ITEMS_WIDTH, obj.ID / WORLD_ITEMS_WIDTH };

		DrawPartialDecal(obj.position, { SPRITE_SCALE * 0.75f,SPRITE_SCALE * 0.75f },
			itemSpriteData->Decal(), spriteCell);
	}

	void DrawWorld()
	{
		// Clear World
		engine->Clear(olc::BLACK);

		// Draw World
		olc::vi2d topLeft = viewPort.GetTopLeftTile().max({ 0,0 });
		olc::vi2d bottomRight = viewPort.GetBottomRightTile().min({ MAP_WIDTH, MAP_HEIGHT });
		olc::vi2d tile;
		for (tile.y = topLeft.y; tile.y < bottomRight.y; tile.y++)
		{
			for (tile.x = topLeft.x; tile.x < bottomRight.x; tile.x++)
			{
				int mapIndex = tile.y * MAP_WIDTH + tile.x;

				DrawTile(mapIndex, tile.x, tile.y);
			}
		}
	}

	void UpdateZoom()
	{
		if (engine->GetMouseWheel() > 0) viewPort.ZoomAtScreenPos(1.5f, engine->GetMousePos());
		if (engine->GetMouseWheel() < 0) viewPort.ZoomAtScreenPos(0.75f, engine->GetMousePos());

		// Clamp the zoom
		if (viewPort.GetWorldScale().x <= MIN_ZOOM)
			viewPort.SetZoom(MIN_ZOOM, camTarget);

		if (viewPort.GetWorldScale().x >= MAX_ZOOM)
			viewPort.SetZoom(MAX_ZOOM, camTarget);

		screenCenter = viewPort.ScaleToWorld(olc::vf2d{ engine->ScreenWidth() / 2.0f, engine->ScreenWidth() / 2.0f });
	}

	void SetCamera(olc::vf2d pos)
	{
		camTarget = pos - screenCenter;
		viewPort.SetWorldOffset(camTarget);
	}
};

#endif

#ifdef OLD_RENDERER

class Renderer
{
public:
	Renderer(olc::PixelGameEngine* engine, WorldData* worldData)
	{
		this->engine = engine;
		this->worldData = worldData;

		// Render scale shit
		// lock the zoomscale to the resolution of 132x108
		ZOOM_SCALE.x = engine->ScreenWidth() / 132.0f;
		ZOOM_SCALE.y = engine->ScreenHeight() / 108.0f;

		// Player shit
		playerSpriteData = new olc::Renderable();
		playerSpriteData->Load("Data/player.png");

		if (playerSpriteData->Decal() == nullptr) {
			print("Could not load player sprites!");
			DEBUG = true;
		}

		print("Loaded player sprites!");

		// Item shit
		itemSpriteData = new olc::Renderable();
		itemSpriteData->Load("Data/items.png");

		// For what ever reason this one specifically is throwing errors
		//if (tileSpriteData->Decal() == nullptr) {
		//	print("Could not load item sprites!");
		//	DEBUG = true;
		//}

		print("Loaded item sprites!");

		// World shit
		tileSpriteData = new olc::Renderable();
		tileSpriteData->Load("Data/WorldTiles.png");

		if (tileSpriteData->Decal() == nullptr) {
			print("Could not load tile sprites!");
			DEBUG = true;
		}

		print("Loaded tile sprites!");

		// Lighting shit
		whiteSquareSprite = new olc::Sprite(SPRITE_SCALE, SPRITE_SCALE);

		for (uint8_t x = 0; x < SPRITE_SCALE; x++)
		{
			for (uint8_t y = 0; y < SPRITE_SCALE; y++)
			{
				whiteSquareSprite->SetPixel(x, y, olc::WHITE);
			}
		}

		whiteSquareDecal = new olc::Decal(whiteSquareSprite);

		tileLightMap = new olc::Pixel[TILE_COUNT];
		lightSources = new LightSource[LIGHT_COUNT];

		for (uint8_t i = 0; i < LIGHT_COUNT; i++)
		{
			tileLightMap[i] = olc::Pixel(0, 0, 0, 255);
			lightSources[i].Initialize({ 0.0f, 0.0f }, tileLightMap[i], 0.5f);
		}
	}

public:
	olc::PixelGameEngine* engine = nullptr;

	olc::TileTransformedView viewPort;

	olc::vf2d cameraPosition = { 0.0,0.0 };
	olc::vf2d targetCameraPosition = { 0.0f, 0.0f };
	const float cameraSpeed = 3.5f;
	olc::vf2d ZOOM_SCALE = { 1.0f, 1.0f };

	olc::Sprite* whiteSquareSprite = nullptr;
	olc::Decal* whiteSquareDecal = nullptr;

	const olc::Pixel SUN_COLOR = { 255, 255, 255, 10 };
	float dayLight = 0;
	bool sunGoingDown = false;
	float sunTimeScale = 300; // Quarterway through the day
	const float SUN_DAY_LENGTH = 600.0f; // 10 minutes

	olc::Pixel* tileLightMap = nullptr;
	LightSource* lightSources = nullptr;

	uint8_t LIGHT_COUNT = 160;

	WorldData* worldData;
	olc::Renderable* tileSpriteData = nullptr;
	olc::Renderable* itemSpriteData = nullptr;
	olc::Renderable* playerSpriteData = nullptr;

	void DrawDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal, olc::Pixel color = { 255,255,255,255 })
	{
		pos *= ZOOM_SCALE;
		engine->DrawDecal(pos - cameraPosition * ZOOM_SCALE, decal, scale * ZOOM_SCALE, color);
	}

	void DrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal, olc::vi2d cell = { 0,0 }, olc::Pixel color = { 255,255,255,255 })
	{
		pos *= ZOOM_SCALE;
		engine->DrawPartialDecal(pos - cameraPosition * ZOOM_SCALE, scale * ZOOM_SCALE, decal, cell, ANIMATION::spriteScale, color);
	}

	void DrawObject(Object* obj) {
		engine->SetPixelMode(olc::Pixel::NORMAL);

		olc::vi2d decalScale = {
			obj->GetLookDir() == ANIMATION::left ? -ANIMATION::spriteScale.x : ANIMATION::spriteScale.x, ANIMATION::spriteScale.y };
		olc::vi2d spriteCell = { ANIMATION::spriteScale.x * obj->GetCellIndex().x, ANIMATION::spriteScale.y * obj->GetCellIndex().y };

		olc::vf2d offset = { obj->GetLookDir() == ANIMATION::left ? ANIMATION::spriteScale.x : 0.0f , 0.0f };

		DrawPartialDecal(obj->GetPosition() + offset, decalScale,
			obj->GetRenderable()->Decal(), spriteCell);
	}

	void DrawItem(Item obj) {
		engine->SetPixelMode(olc::Pixel::NORMAL);

		olc::vi2d spriteCell{ obj.ID % WORLD_ITEMS_WIDTH, obj.ID / WORLD_ITEMS_WIDTH };

		DrawPartialDecal(obj.position, { SPRITE_SCALE * 0.75f,SPRITE_SCALE * 0.75f },
			itemSpriteData->Decal(), spriteCell);
	}

	void DrawWorld()
	{
		engine->SetPixelMode(olc::Pixel::NORMAL);

		worldData->MoveTiles(cameraPosition);

		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			auto& tile = worldData->tiles[i];

			olc::vf2d pos = { float(tile.x),  float(tile.y) };
			pos *= ANIMATION::spriteScale;

			int cellIndex_x = tile.tileID % WORLD_TILES_WIDTH;
			int cellIndex_y = tile.tileID / WORLD_TILES_WIDTH;

			olc::vi2d tileSpriteCell = { ANIMATION::spriteScale.x * cellIndex_x, ANIMATION::spriteScale.y * cellIndex_y };
			DrawPartialDecal(pos, ANIMATION::spriteScale, tileSpriteData->Decal(), tileSpriteCell);

			uint8_t foliage = tile.foliageID;
			if (foliage == 0) continue; // 0 = no foliage
			foliage -= 1; // Subtract 1 to put this layer back into sprite space

			olc::vi2d foliageSpriteCell = { ANIMATION::spriteScale.x * foliage, ANIMATION::spriteScale.x * 2 };
			DrawPartialDecal(pos, ANIMATION::spriteScale, tileSpriteData->Decal(), foliageSpriteCell);
		}
	}

	void UpdateLights()
	{
		engine->SetPixelMode(olc::Pixel::ALPHA);

		// Update all the lights
		for (uint8_t i = 0; i < LIGHT_COUNT; i++)
		{
			olc::Pixel color = lightSources[i].color;

			color.a = 255.0f * lightSources[i].intensity;
			tileLightMap[i] = color;
		}

		olc::Pixel sunColor = (SUN_COLOR * dayLight);

		// Draw the lights
		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			auto& tile = worldData->tiles[i];

			olc::vf2d pos = { float(tile.x),  float(tile.y) };
			pos *= ANIMATION::spriteScale;

			//if (pos.y >= engine->ScreenHeight() || pos.y < 0 || pos.x >= engine->ScreenWidth() || pos.x < 0) continue;

			olc::Pixel lightMapColor = (tileLightMap[i]);
			lightMapColor.a *= 1 - dayLight;

			DrawDecal(pos, { 1,1 }, whiteSquareDecal, lightMapColor);
		}

		engine->SetPixelMode(olc::Pixel::NORMAL);
	}

	void SnapCamera(olc::vf2d targetPosition)
	{
		targetCameraPosition = targetPosition - ANIMATION::spriteScale * 5;
		cameraPosition = targetCameraPosition;
	}

	void UpdateCameraPosition(olc::vf2d targetPosition, float fElapsedTime)
	{
		targetCameraPosition = targetPosition - ANIMATION::spriteScale * 5;
		olc::vf2d moveAmount = (targetCameraPosition - cameraPosition) * cameraSpeed * fElapsedTime;

		cameraPosition += moveAmount;
	}

	void UpdateSun(float fElapsedTime)
	{
		sunTimeScale += sunGoingDown ? -fElapsedTime : fElapsedTime;
		if (sunTimeScale >= SUN_DAY_LENGTH)
		{
			sunGoingDown = true;
		}
		else if (sunTimeScale <= 0)
		{
			sunGoingDown = false;
		}

		dayLight = sunTimeScale / SUN_DAY_LENGTH;
		//engine->DrawStringDecal({ 0.0,0.0 }, std::to_string(dayLight)); // Show amount of sunlight
	}
};

#endif