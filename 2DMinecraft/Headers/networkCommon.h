#pragma once
#include <cstdint>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_NETWORK
#include "olcPGEX_Network.h"

enum class GameMsg : uint32_t
{
	Server_GetStatus,
	Server_GetPing,

	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,

	Game_AddPlayer,
	Game_RemovePlayer,
	Game_UpdatePlayer,
};

struct PlayerDescription
{
	uint32_t uniqueID = 0;
	uint32_t avatarID = 0;

	uint32_t health = 100;
	uint32_t ammo = 20;
	uint32_t kills = 0;
	uint32_t deaths = 0;

	float radius = 0.5f;

	olc::vf2d position;
	olc::vf2d velocity;
};