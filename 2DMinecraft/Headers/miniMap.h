// Forrest Lowe 2021

#pragma once

#ifndef MINI_MAP
#define MINI_MAP

#include "olcPixelGameEngine.h"
#include "worldData.h"
#include "timeConstruct.h"

class MiniMap
{
private:
	const uint8_t PLAYER_ICON_SCALE = 25;

	const olc::Key TOGGLE_KEY = olc::Key::M;
	const olc::Key SCALE_IN_KEY = olc::Key::I;
	const olc::Key SCALE_OUT_KEY = olc::Key::O;
	const olc::Key MOVE_MAP_KEY = olc::Key::L;

	WorldData* worldData = nullptr;
	olc::PixelGameEngine* engine = nullptr;
	TimeConstruct* time = nullptr;

	olc::Sprite* miniMapSprite;
	olc::Decal* miniMapDecal;

	bool drawMiniMap = false;
	uint8_t miniMapDrawLocation = 1;
	float miniMapDrawScale = 0.25f;
	olc::Pixel* mapColors;

	void UpdateDecal(olc::vf2d localPlayer);

public:
	void Initialize(olc::Sprite* tileSprite, WorldData* worldData,
		olc::PixelGameEngine* engine, TimeConstruct* time);
	void UpdateMiniMap(olc::vf2d localPlayer);
};

#endif // !MINI_MAP