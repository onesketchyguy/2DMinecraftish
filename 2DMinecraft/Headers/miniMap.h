// Forrest Lowe 2021

#pragma once

#ifndef MINI_MAP
#define MINI_MAP

#include "olcPixelGameEngine.h"
#include "worldData.h"
#include "timeConstruct.h"
#include "bonusMaths.h"

class MiniMap
{
private:
	const uint8_t PLAYER_ICON_SCALE = 5;
	const uint8_t CLEAN_AREA_SCALE = 10;

	const olc::Key TOGGLE_KEY = olc::Key::M;

	WorldData* worldData = nullptr;
	olc::PixelGameEngine* engine = nullptr;
	TimeConstruct* time = nullptr;

	olc::Sprite* cachedMap;
	olc::Sprite* miniMapSprite;
	olc::Decal* miniMapDecal;

	bool drawMiniMap = false;
	bool updateMap = true;
	float miniMapDrawScale = 0.25f;
	olc::Pixel* mapColors;
	olc::Pixel playerColor = olc::Pixel(255, 0, 0, 255);

	olc::Pixel GetDitherColor(int x, int y, float mutator, double4 error);
	void UpdateSprite();
	void UpdateDecal(olc::vf2d localPlayer);

public:
	void Initialize(olc::Sprite* tileSprite, WorldData* worldData,
		olc::PixelGameEngine* engine, TimeConstruct* time);
	void UpdateMiniMap(olc::vf2d localPlayer);
	const bool GetDrawingMiniMap() const
	{
		return drawMiniMap;
	}

public:
	~MiniMap()
	{
		delete miniMapSprite;
		delete miniMapDecal;
		delete mapColors;
	}
};

#endif // !MINI_MAP