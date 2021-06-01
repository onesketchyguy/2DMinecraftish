// Forrest Lowe 2021

#pragma once

#ifndef CONSTANTS
#define CONSTANTS

#include "olcPixelGameEngine.h"

extern bool DEBUG;
extern bool APPLICATION_RUNNING;

// Sprite stuff
extern const int8_t SPRITE_SCALE;
extern olc::vi2d spriteScale;

extern const uint8_t WATER_SPRITE_INDEX;

extern const uint8_t WORLD_TILES_WIDTH;
extern const uint8_t WORLD_TILES_HEIGHT;

extern const uint8_t WORLD_ITEMS_WIDTH;
extern const uint8_t WORLD_ITEMS_HEIGHT;

extern const uint8_t TILE_COUNT;

// Multiplayer stuff
extern std::string serverIP;
extern const int SERVER_PORT;

// Gameplay stuff
enum class LOOK_DIR : uint8_t
{
	down = 0,
	right = 1,
	up = 2,
	left = 3
};

enum class PLAY_MODE : uint8_t
{
	SINGLE_PLAYER,
	CLIENT,
	SERVER
};

// Current scene stuff
enum class SCENE : uint8_t
{
	SCENE_INTRO,
	SCENE_MAIN_MENU,
	SCENE_GAME
};

inline PLAY_MODE playMode = PLAY_MODE::SINGLE_PLAYER;
inline SCENE currentScene = SCENE::SCENE_INTRO;

#endif // !CONSTANTS