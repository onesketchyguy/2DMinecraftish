// Forrest Lowe 2021

#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

#include "Headers/uiObjects.h"
#include "Headers/debugger.h"
#include "networkCommon.h"
#include "ConstantData.h"
#include "timeConstruct.h"
#include "worldData.h"
#include "miniMap.h"
#include "objectDefinitions.h"

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

	const float MIN_ZOOM = 10.0f;
	const float MAX_ZOOM = 36.0f;

	olc::vi2d topLeft;
	olc::vi2d bottomRight;

public:
	Renderer(olc::PixelGameEngine* engine, WorldData* worldData);

public:
	olc::PixelGameEngine* engine = nullptr;

	olc::TileTransformedView viewPort;

	WorldData* worldData = nullptr;
	olc::Renderable* tileSpriteData = nullptr;
	olc::Renderable* itemSpriteData = nullptr;
	olc::Renderable* playerSpriteData = nullptr;

	std::vector<DecalData> drawQueue;

	void LoadSprites(olc::Renderable* renderable, std::string dir);
	void DrawDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::Pixel color = { 255,255,255,255 });
	void DrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::vi2d cell = { 0,0 }, olc::Pixel color = { 255,255,255,255 });
	void DrawQueue(bool clearOnDrawn = true);
	void EnqueueDrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::vi2d cell = { 0,0 }, olc::Pixel color = { 255,255,255,255 });

	void DrawPartialDecalInScreenSpace(olc::vf2d pos, olc::vf2d scale,
		olc::Decal* decal, olc::vi2d cell = { 0,0 }, olc::vf2d sprScale = spriteScale,
		olc::Pixel color = { 255,255,255,255 });
	void DrawPlayer(PlayerDescription& player);
	void DrawTile(int mapIndex, float x, float y);
	void EnqueueDrawTile(int mapIndex, float x, float y);
	void DrawItem(Item obj);
	void DrawWorld();
	void UpdateZoom();
	void SetCamera(olc::vf2d pos);
};

Renderer::Renderer(olc::PixelGameEngine* engine, WorldData* worldData)
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

void Renderer::LoadSprites(olc::Renderable* renderable, std::string dir)
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

void Renderer::DrawDecal(olc::vf2d pos, olc::vf2d scale,
	olc::Decal* decal, olc::Pixel color)
{
	viewPort.DrawDecal(pos, decal, scale, color);
}

void Renderer::DrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
	olc::vi2d cell, olc::Pixel color)
{
	viewPort.DrawPartialDecal(pos, scale, decal, cell, spriteScale, color);
}

void Renderer::DrawQueue(bool clearOnDrawn)
{
	for (DecalData& data : drawQueue)
	{
		viewPort.DrawPartialDecal(data.pos, data.scale, data.decal, data.cell,
			spriteScale, data.color);
	}

	if (clearOnDrawn)
	{
		drawQueue.clear();
	}
}

void Renderer::EnqueueDrawPartialDecal(olc::vf2d pos, olc::vf2d scale,
	olc::Decal* decal, olc::vi2d cell, olc::Pixel color)
{
	DecalData data(pos, scale, decal, cell, color);

	drawQueue.push_back(data);
}

void Renderer::DrawPartialDecalInScreenSpace(olc::vf2d pos, olc::vf2d scale,
	olc::Decal* decal, olc::vi2d cell, olc::vf2d sprScale, olc::Pixel color)
{
	olc::vf2d worldPos = viewPort.ScreenToWorld(pos);
	viewPort.DrawPartialDecal(worldPos, scale, decal, cell, sprScale, color);
}

void Renderer::DrawPlayer(PlayerDescription& player)
{
	engine->SetPixelMode(olc::Pixel::NORMAL);

	olc::vf2d sprScale{ static_cast<float>(spriteScale.x), static_cast<float>(spriteScale.y) };
	olc::vf2d pos = player.position;

	if (player.flip_x)
	{
		pos.x += sprScale.x;
		sprScale.x *= -1;
	}

	DrawPartialDecal(pos / static_cast<float>(SPRITE_SCALE),
		sprScale, playerSpriteData->Decal(),
		{
			static_cast<int>(player.avatarIndex_x * SPRITE_SCALE),
			static_cast<int>(player.avatarIndex_y * SPRITE_SCALE)
		});
}

void Renderer::DrawTile(int mapIndex, float x, float y)
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

void Renderer::EnqueueDrawTile(int mapIndex, float x, float y)
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

void Renderer::DrawItem(Item obj) {
	engine->SetPixelMode(olc::Pixel::NORMAL);

	olc::vi2d spriteCell{ obj.ID % WORLD_ITEMS_WIDTH, obj.ID / WORLD_ITEMS_WIDTH };

	DrawPartialDecal(obj.position, { SPRITE_SCALE * 0.75f,SPRITE_SCALE * 0.75f },
		itemSpriteData->Decal(), spriteCell);
}

void Renderer::DrawWorld()
{
	// Clear World
	engine->Clear(olc::BLACK);

	// Draw World
	topLeft = viewPort.GetTopLeftTile().max({ 0,0 });
	bottomRight = viewPort.GetBottomRightTile().min({ worldData->GetMapWidth(), worldData->GetMapHeight() });
	olc::vi2d tile;
	for (tile.y = topLeft.y; tile.y < bottomRight.y; tile.y++)
	{
		for (tile.x = topLeft.x; tile.x < bottomRight.x; tile.x++)
		{
			int mapIndex = tile.y * worldData->GetMapWidth() + tile.x;

			float x = static_cast<float>(tile.x);
			float y = static_cast<float>(tile.y);

			EnqueueDrawTile(mapIndex, x, y);
		}
	}

	DrawQueue();
}

void Renderer::UpdateZoom()
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

void Renderer::SetCamera(olc::vf2d pos)
{
	camTarget = pos - screenCenter;
	viewPort.SetWorldOffset(camTarget);
}

#endif // !RENDERER_H