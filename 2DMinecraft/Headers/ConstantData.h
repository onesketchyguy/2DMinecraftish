// Forrest Lowe 2021

#pragma once

#ifndef CONSTANTS
#define CONSTANTS

#include "olcPixelGameEngine.h"

bool DEBUG = false;
bool APPLICATION_RUNNING = true;

const int8_t SPRITE_SCALE = 12;
olc::vi2d spriteScale = { (int)SPRITE_SCALE, (int)SPRITE_SCALE };

const uint8_t WORLD_TILES_WIDTH = 4;
const uint8_t WORLD_TILES_HEIGHT = 3;

const uint8_t WORLD_ITEMS_WIDTH = 3;
const uint8_t WORLD_ITEMS_HEIGHT = 1;

std::string serverIP = "127.0.0.1";
const int SERVER_PORT = 60000;

enum class LOOK_DIR : uint8_t
{
	down = 0,
	right = 1,
	up = 2,
	left = 3
};

#endif // !CONSTANTS