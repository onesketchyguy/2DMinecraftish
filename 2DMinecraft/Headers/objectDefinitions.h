#pragma once

class AABBCollider
{
public:
	AABBCollider() = default;

	AABBCollider(olc::Sprite* sprite)
	{
		RecalculateBounds(sprite);
	}

	AABBCollider(std::wstring spriteData, int spriteWidth, int spriteHeight)
	{
		this->posX = this->posY = 0;

		this->width = -1;
		this->height = -1;

		this->offX = spriteWidth;
		this->offY = spriteHeight;

		for (size_t i = 0; i < spriteWidth * spriteHeight; i++)
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

	AABBCollider(int width, int height)
	{
		this->width = width;
		this->height = height;
	}

public:
	int width = 0, height = 0;
	int offX = 0, offY = 0;

	int posX = 0, posY = 0;

public:
	int GetTop() {
		return posY + offY;
	}

	int GetBottom() {
		return posY + offY + height;
	}

	int GetMiddleY() {
		return (int)(posY + offY + (height / 2.0f));
	}

	int GetLeft() {
		return posX + offX;
	}

	int GetRight() {
		return posX + offX + width;
	}

	int GetMiddleX() {
		return (int)(posX + offX + (width / 2.0f));
	}

public:
	void UpdatePosition(int x, int y)
	{
		posX = x;
		posY = y;
	}

	bool CheckOverlaps(int x, int y) {
		bool withinY = y <= GetBottom() && y >= GetTop();
		bool withinX = x >= GetLeft() && x <= GetRight();

		return withinX && withinY;
	}

	bool CheckCollision(AABBCollider* other) {
		int otherRight = other->GetRight();
		int otherLeft = other->GetLeft();

		int otherTop = other->GetTop();
		int otherBot = other->GetBottom();

		bool withinY = (otherTop <= GetBottom() && otherTop >= GetTop()) || (otherBot <= GetBottom() && otherBot >= GetTop());
		bool withinX = (otherRight >= GetLeft() && otherRight <= GetRight()) || (otherLeft >= GetLeft() && otherLeft <= GetRight());

		return withinX && withinY;
	}

	void RecalculateBounds(olc::Sprite* sprite) {
		this->posX = this->posY = 0;

		this->width = -1;
		this->height = -1;

		this->offX = sprite->width;
		this->offY = sprite->height;

		olc::Pixel* spriteData = sprite->GetData();

		for (size_t i = 0; i < sprite->width * sprite->height; i++)
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
};

class Object {
private:
	olc::Sprite* sprite;
	olc::Decal* decal;

	olc::PixelGameEngine* engine;

	olc::vf2d position;

	AABBCollider* collider;

public:
	void Draw() {
		engine->SetPixelMode(olc::Pixel::NORMAL);
		engine->DrawDecal(position, decal);
	}

	bool CheckCollision(AABBCollider* collider)
	{
		return this->collider->CheckCollision(collider);
	}

	void SetPosition(olc::vf2d position)
	{
		this->position = position;
	}

	olc::vf2d GetPosition()
	{
		return this->position;
	}

public:
	Object() = default;
	Object(olc::Sprite* sprite, olc::vf2d position = { 0.0f, 0.0f })
	{
		// Generate the sprite/decal
		this->sprite = this->sprite->Duplicate();
		this->decal = new olc::Decal(this->sprite);

		// Set the position of this object
		this->position = position;

		//Generate a new collider
		this->collider = new AABBCollider(this->sprite);
	}
};