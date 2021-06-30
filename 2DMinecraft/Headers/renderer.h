// Forrest Lowe 2021

#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "olcPGEX_TransformedView.h"

#include "uiObjects.h"
#include "debugger.h"
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
	const float MIN_ZOOM = 10.0f;
	const float MAX_ZOOM = 26.0f;

	olc::vf2d screenCenter{ 0.0f, 0.0f };
	olc::vf2d camTarget{ 0.0f, 0.0f };

	olc::vi2d topLeft{ 0, 0 };
	olc::vi2d bottomRight{ 0, 0 };

public:
	Renderer(olc::PixelGameEngine* engine);
	~Renderer()
	{
		delete squareSprite;
		delete squareDecal;
		delete tileSpriteData;
		delete itemSpriteData;
		delete playerSpriteData;
		delete resourcePack;
		delete toolsRenderable;
		delete itemSlotRenderable;

		drawQueue.clear();
	}

public:
	olc::PixelGameEngine* engine = nullptr;

	olc::TileTransformedView viewPort;

	WorldData* worldData = nullptr;

	olc::ResourcePack* resourcePack = nullptr;
	olc::Sprite* squareSprite = nullptr;
	olc::Decal* squareDecal = nullptr;
	olc::Renderable* tileSpriteData = nullptr;
	olc::Renderable* itemSpriteData = nullptr;
	olc::Renderable* playerSpriteData = nullptr;
	olc::Renderable* toolsRenderable = nullptr;
	olc::Renderable* worldToolsRenderable = nullptr;
	olc::Renderable* itemSlotRenderable = nullptr;

	std::vector<DecalData> drawQueue;

	void ClearWorldData()
	{
		this->worldData = nullptr;
	}

	void SetWorldData(WorldData* worldData)
	{
		this->worldData = worldData;
	}

	void LoadSprites(olc::Renderable* renderable, std::string dir);
	void DrawDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::Pixel color = { 255,255,255,255 });
	void DrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
		olc::vi2d cell = { 0,0 }, olc::Pixel color = { 255,255,255,255 });
	void DrawQueue();
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
	void SetZoomScaleToMax();
	void SetZoomScaleToMin();
	void UpdateZoom();
	void SetCamera(olc::vf2d pos);
};

#endif // !RENDERER_H