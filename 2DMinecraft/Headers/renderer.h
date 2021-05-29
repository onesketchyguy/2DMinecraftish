#pragma once

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

struct DecalData
{
public:
	olc::vf2d pos = { 0, 0 };
	olc::vf2d scale = { 0, 0 };
	olc::Decal* decal = nullptr;
	olc::vi2d cell = { 0,0 };
	olc::Pixel color = { 255,255,255,255 };

	DecalData() = default;
	DecalData(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::vi2d cell = { 0,0 }, olc::Pixel color = { 255,255,255,255 })
	{
		this->pos = pos;
		this->scale = scale;
		this->decal = decal;
		this->cell = cell;
		this->color = color;
	}
};

class Renderer
{
private:
	olc::vf2d screenCenter;
	olc::vf2d camTarget;

	const float MIN_ZOOM = 6.0f;
	const float MAX_ZOOM = 36.0f;

	olc::vi2d topLeft;
	olc::vi2d bottomRight;

public:
	Renderer(olc::PixelGameEngine* engine, WorldData* worldData)
	{
		this->engine = engine;
		this->worldData = worldData;

		viewPort = olc::TileTransformedView({ engine->ScreenWidth(), engine->ScreenHeight() },
			{ SPRITE_SCALE, SPRITE_SCALE });

		print("Created viewport.");

		print("Generating sprites...");

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

	std::vector<DecalData> drawQueue;

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

	void DrawDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::Pixel color = { 255,255,255,255 })
	{
		viewPort.DrawDecal(pos, decal, scale, color);
	}

	void DrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::vi2d cell = { 0,0 }, olc::Pixel color = { 255,255,255,255 })
	{
		viewPort.DrawPartialDecal(pos, scale, decal, cell, spriteScale, color);
	}

	void DrawQueue(bool clearOnDrawn = true)
	{
		for (DecalData& data : drawQueue)
		{
			viewPort.DrawPartialDecal(data.pos, data.scale, data.decal, data.cell, spriteScale, data.color);
		}

		if (clearOnDrawn)
		{
			drawQueue.clear();
		}
	}

	void EnqueueDrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::vi2d cell = { 0,0 }, olc::Pixel color = { 255,255,255,255 })
	{
		DecalData data(pos, scale, decal, cell, color);

		drawQueue.push_back(data);
	}

	void DrawPartialDecalInScreenSpace(olc::vf2d pos, olc::vf2d scale,
		olc::Decal* decal, olc::vi2d cell = { 0,0 }, olc::vf2d sprScale = spriteScale,
		olc::Pixel color = { 255,255,255,255 })
	{
		olc::vf2d worldPos = viewPort.ScreenToWorld(pos);
		viewPort.DrawPartialDecal(worldPos, scale, decal, cell, sprScale, color);
	}

	void DrawObject(Object* obj)
	{
		engine->SetPixelMode(olc::Pixel::NORMAL);

		olc::vi2d decalScale = {
			obj->GetLookDir() == LOOK_DIR::left ? -spriteScale.x : spriteScale.x, spriteScale.y };
		olc::vi2d spriteCell = { spriteScale.x * obj->GetCellIndex().x, spriteScale.y * obj->GetCellIndex().y };

		olc::vf2d offset = { obj->GetLookDir() == LOOK_DIR::left ? spriteScale.x : 0.0f , 0.0f };

		DrawPartialDecal(obj->GetPosition() + offset, decalScale,
			obj->GetRenderable()->Decal(), spriteCell);
	}

	void DrawPlayer(olc::vf2d pos)
	{
		engine->SetPixelMode(olc::Pixel::NORMAL);

		DrawPartialDecal(pos, spriteScale, playerSpriteData->Decal(), { 0, 0 });
	}

	void DrawTile(int mapIndex, float x, float y)
	{
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
		DrawPartialDecal(pos, spriteScale + olc::vf2d{ 1.5f, 1.5f }, tileSpriteData->Decal(), tileSpriteCell);

		if (foliage > 0) // 0 = no foliage
		{
			foliage -= 1; // Subtract 1 to put this layer back into sprite space

			olc::vi2d foliageSpriteCell = { spriteScale.x * foliage, spriteScale.x * 2 };

			// Draw foliage
			DrawPartialDecal(pos, spriteScale, tileSpriteData->Decal(), tileSpriteCell);
		}
	}

	void EnqueueDrawTile(int mapIndex, float x, float y)
	{
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
		EnqueueDrawPartialDecal(pos, spriteScale + olc::vf2d{ 1.5f, 1.5f }, tileSpriteData->Decal(), tileSpriteCell);

		if (foliage > 0) // 0 = no foliage
		{
			foliage -= 1; // Subtract 1 to put this layer back into sprite space

			olc::vi2d foliageSpriteCell = { spriteScale.x * foliage, spriteScale.x * 2 };

			// Draw foliage
			EnqueueDrawPartialDecal(pos, spriteScale, tileSpriteData->Decal(), tileSpriteCell);
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
		topLeft = viewPort.GetTopLeftTile().max({ 0,0 });
		bottomRight = viewPort.GetBottomRightTile().min({ MAP_WIDTH, MAP_HEIGHT });
		olc::vi2d tile;
		for (tile.y = topLeft.y; tile.y < bottomRight.y; tile.y++)
		{
			for (tile.x = topLeft.x; tile.x < bottomRight.x; tile.x++)
			{
				int mapIndex = tile.y * MAP_WIDTH + tile.x;

				EnqueueDrawTile(mapIndex, tile.x, tile.y);
			}
		}

		DrawQueue();
	}

	void UpdateZoom()
	{
		if (engine->GetMouseWheel() > 0) viewPort.ZoomAtScreenPos(1.5f, engine->GetMousePos());
		if (engine->GetMouseWheel() < 0) viewPort.ZoomAtScreenPos(0.5f, engine->GetMousePos());

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