// Forrest Lowe 2021
#include "../Headers/AABBCollider.h"

AABBCollider::AABBCollider(olc::Sprite* sprite)
{
	RecalculateBounds(sprite);
}

AABBCollider::AABBCollider(std::wstring spriteData, int spriteWidth, int spriteHeight)
{
	this->posX = this->posY = 0;

	this->width = -1;
	this->height = -1;

	this->offX = spriteWidth;
	this->offY = spriteHeight;

	int spriteLength = spriteWidth * spriteHeight;

	for (int i = 0; i < spriteLength; i++)
	{
		int ix = i % spriteWidth;
		int iy = i / spriteWidth;

		if (spriteData[i] != '.')
		{
			// Calculate the start position
			if (ix < this->offX)
			{
				this->offX = ix;
			}

			if (iy < this->offY)
			{
				this->offY = iy;
			}

			// Calculate the bounds
			if (ix - this->offX > this->width)
			{
				this->width = ix - this->offX;
			}

			if (iy - this->offY > this->height)
			{
				this->height = iy - this->offY;
			}
		}
	}
}

AABBCollider::AABBCollider(int width, int height)
{
	this->width = width;
	this->height = height;
}

void AABBCollider::UpdatePosition(int x, int y)
{
	posX = x;
	posY = y;
}

bool AABBCollider::CheckOverlaps(int x, int y)
{
	bool withinY = y <= GetBottom() && y >= GetTop();
	bool withinX = x >= GetLeft() && x <= GetRight();

	return withinX && withinY;
}

bool AABBCollider::CheckCollision(AABBCollider* other)
{
	int otherRight = other->GetRight();
	int otherLeft = other->GetLeft();

	int otherTop = other->GetTop();
	int otherBot = other->GetBottom();

	bool withinY = (otherTop <= GetBottom() && otherTop >= GetTop()) || (otherBot <= GetBottom() && otherBot >= GetTop());
	bool withinX = (otherRight >= GetLeft() && otherRight <= GetRight()) || (otherLeft >= GetLeft() && otherLeft <= GetRight());

	return withinX && withinY;
}

void AABBCollider::RecalculateBounds(olc::Sprite* sprite)
{
	this->posX = this->posY = 0;

	this->width = -1;
	this->height = -1;

	this->offX = sprite->width;
	this->offY = sprite->height;

	int spriteLength = sprite->width * sprite->height;

	olc::Pixel* spriteData = sprite->GetData();

	for (int i = 0; i < spriteLength; i++)
	{
		int ix = i % sprite->width;
		int iy = i / sprite->width;

		if (spriteData[i] != 0)
		{
			// Calculate the start position
			if (ix < this->offX)
			{
				this->offX = ix;
			}

			if (iy < this->offY)
			{
				this->offY = iy;
			}

			// Calculate the bounds
			if (ix - this->offX > this->width)
			{
				this->width = ix - this->offX;
			}

			if (iy - this->offY > this->height)
			{
				this->height = iy - this->offY;
			}
		}
	}
}