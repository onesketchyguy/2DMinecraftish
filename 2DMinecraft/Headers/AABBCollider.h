#ifndef AABBCOLLIDER
#define AABBCOLLIDER

#include "olcPixelGameEngine.h"

class AABBCollider
{
public:
	AABBCollider() = default;
	AABBCollider(olc::Sprite* sprite);
	AABBCollider(std::wstring spriteData, int spriteWidth, int spriteHeight);
	AABBCollider(int width, int height);

public:
	int width = 0, height = 0;
	int offX = 0, offY = 0;

	int posX = 0, posY = 0;

public:
	int GetTop() { return posY + offY; }
	int GetBottom() { return posY + offY + height; }
	int GetMiddleY() { return static_cast<int>(posY + offY + (height / 2.0f)); }
	int GetLeft() { return posX + offX; }
	int GetRight() { return posX + offX + width; }
	int GetMiddleX() { return static_cast<int>(posX + offX + (width / 2.0f)); }

public:
	void UpdatePosition(int x, int y);
	bool CheckOverlaps(int x, int y);
	bool CheckCollision(AABBCollider* other);
	void RecalculateBounds(olc::Sprite* sprite);
};

#endif