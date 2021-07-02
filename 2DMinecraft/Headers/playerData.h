#pragma once

#ifndef PLAYER_DATA
#define PLAYER_DATA
#include "networkCommon.h"

const int TOOLS_COUNT = 7;
enum class Tools : uint32_t
{
	None,
	Shovel,
	Pickaxe,
	Axe,
	Hoe,
	Sword,
	FishingPole,
};

inline const char* ParseToolToString(Tools item)
{
	switch (item)
	{
	case Tools::Shovel:
		return "shovel";
	case Tools::Pickaxe:
		return "pickaxe";
	case Tools::Axe:
		return "axe";
	case Tools::Hoe:
		return  "hoe";
	case Tools::Sword:
		return  "sword";
	case Tools::FishingPole:
		return  "fishing pole";
	default:
		return "Non-item";
	}
}

inline const Tools ParseStringToTool(const char* string)
{
	if (string == "shovel") return Tools::Shovel;
	if (string == "pickaxe")return Tools::Pickaxe;
	if (string == "axe")return Tools::Axe;
	if (string == "hoe")return Tools::Hoe;
	if (string == "sword") return Tools::Sword;
	if (string == "fishing pole") return Tools::FishingPole;

	return Tools::None;
}

class PlayerObject
{
public:
	PlayerDescription* desc;
	Tools currentTool = Tools::None;

	olc::vi2d cellIndex = { 0, 0 };

	LOOK_DIR lookDir = LOOK_DIR::down;

	void Update(float fElapsedTime)
	{
		if (desc->velocity.x > 0)
		{
			// Should face right
			lookDir = LOOK_DIR::right;
		}
		else if (desc->velocity.x < 0)
		{
			// Should face left
			lookDir = LOOK_DIR::left;
		}

		if (desc->velocity.y > 0)
		{
			// Should face down
			lookDir = LOOK_DIR::down;
		}
		else if (desc->velocity.y < 0)
		{
			// Should face up
			lookDir = LOOK_DIR::up;
		}

		int waterIndex = desc->inWater ? WATER_SPRITE_INDEX : 0;

		switch (lookDir)
		{
		case LOOK_DIR::down:
			cellIndex = { waterIndex + 2, 0 };
			break;
		case LOOK_DIR::right:
			cellIndex = { waterIndex + 0, 0 };
			break;
		case LOOK_DIR::up:
			cellIndex = { waterIndex + 1, 0 };
			break;
		case LOOK_DIR::left:
			cellIndex = { waterIndex + 0, 0 };
			break;
		default:
			break;
		}

		desc->avatarIndex_x = cellIndex.x;
		desc->avatarIndex_y = cellIndex.y;

		desc->flip_x = lookDir == LOOK_DIR::left;
	}

public:
	PlayerObject(PlayerDescription* desc)
	{
		this->desc = desc;
	}
};

#endif // !PLAYER_DATA