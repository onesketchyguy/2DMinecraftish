#pragma once

namespace ANIMATION {
	const int8_t SPRITE_SCALE = 12;
	olc::vi2d spriteScale = { (int)SPRITE_SCALE, (int)SPRITE_SCALE };

	enum LOOK_DIR {
		down = 0,
		right = 1,
		up = 2,
		left = 3
	};
}

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
	olc::Renderable* renderable = nullptr;
	olc::vi2d cellIndex = { 0, 0 };

	ANIMATION::LOOK_DIR lookDir = ANIMATION::down;

	olc::PixelGameEngine* engine;

	olc::vf2d position;

	AABBCollider* collider;
public:
	olc::vf2d velocity = { 0.0, 0.0 };

public:
	void Update(float fElapsedTime)
	{
		if (velocity.x > 0) {
			// Should face right
			lookDir = ANIMATION::right;
		}
		else if (velocity.x < 0) {
			// Should face left
			lookDir = ANIMATION::left;
		}

		if (velocity.y > 0) {
			// Should face down
			lookDir = ANIMATION::down;
		}
		else if (velocity.y < 0) {
			// Should face up
			lookDir = ANIMATION::up;
		}

		switch (lookDir)
		{
		case ANIMATION::down:
			cellIndex = { 2, 0 };
			break;
		case ANIMATION::right:
			cellIndex = { 0, 0 };
			break;
		case ANIMATION::up:
			cellIndex = { 1, 0 };
			break;
		case ANIMATION::left:
			cellIndex = { 0, 0 };
			break;
		default:
			break;
		}

		this->position += velocity;
		velocity *= 0.9f;
	}

	void Draw(olc::vf2d CameraPosition) {
		engine->SetPixelMode(olc::Pixel::NORMAL);

		olc::vi2d decalScale = {
			lookDir == ANIMATION::left ? -ANIMATION::spriteScale.x : ANIMATION::spriteScale.x, ANIMATION::spriteScale.y };
		olc::vi2d spriteCell = { ANIMATION::spriteScale.x * cellIndex.x, ANIMATION::spriteScale.y * cellIndex.y };

		olc::vf2d offset = { lookDir == ANIMATION::left ? ANIMATION::spriteScale.x : 0.0f , 0.0f };

		engine->DrawPartialDecal(this->position + offset - CameraPosition,
			decalScale,
			renderable->Decal(), spriteCell, ANIMATION::spriteScale);
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
	Object(olc::Renderable* renderable, olc::PixelGameEngine* engine = nullptr, olc::vf2d position = { 0.0f, 0.0f })
	{
		// Generate the sprite/decal
		this->renderable = renderable;

		// Set the position of this object
		this->position = position;

		// Set the engine for future rendering
		this->engine = engine;

		//Generate a new collider
		this->collider = new AABBCollider(12, 12);
	}
};

struct LightSource
{
public:
	olc::vf2d position;
	olc::Pixel color;
	float intensity;

	void Initialize(olc::vf2d position, olc::Pixel color, float intensity) {
		this->position = position;
		this->color = color;
		this->intensity = intensity;
	}
};