#pragma once

enum LOOK_DIR
{
	down = 0,
	right = 1,
	up = 2,
	left = 3
};

#include "AABBCollider.h"

class Object
{
private:
	AABBCollider* collider;

	olc::Renderable* renderable = nullptr;
	olc::vi2d cellIndex = { 0, 0 };

	const byte WATER_INDEX = 3;

	LOOK_DIR lookDir = LOOK_DIR::down;

	olc::vf2d position;
public:
	olc::vf2d velocity = { 0.0, 0.0 };

	bool inWater = false;

public:
	void Update(float fElapsedTime)
	{
		if (velocity.x > 0) {
			// Should face right
			lookDir = LOOK_DIR::right;
		}
		else if (velocity.x < 0) {
			// Should face left
			lookDir = LOOK_DIR::left;
		}

		if (velocity.y > 0) {
			// Should face down
			lookDir = LOOK_DIR::down;
		}
		else if (velocity.y < 0) {
			// Should face up
			lookDir = LOOK_DIR::up;
		}

		int waterIndex = inWater ? WATER_INDEX : 0;

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

		this->position += velocity;
		velocity *= 0.9f;
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

	LOOK_DIR GetLookDir()
	{
		return lookDir;
	}

	olc::Renderable* GetRenderable()
	{
		return renderable;
	}

	olc::vi2d GetCellIndex()
	{
		return cellIndex;
	}

public:
	Object() = default;
	Object(olc::Renderable* renderable, olc::vf2d position = { 0.0f, 0.0f })
	{
		// Generate the sprite/decal
		this->renderable = renderable;

		// Set the position of this object
		this->position = position;

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

#include "Item.h"