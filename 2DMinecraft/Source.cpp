bool CRASH = false;

#define OLC_PGE_APPLICATION
#include "Headers/olcPixelGameEngine.h"
#include "Headers/tileDefinitions.h"

const int8_t SPRITE_SCALE = 12;
const uint8_t TILE_COUNT = 216; // The constant for how many MAX tiles can be drawn at once
#include "Headers/worldData.h"
#include "Headers/objectDefinitions.h"
#include "Headers/renderer.h"

// Override base class with your custom functionality
class MiniMinecraft : public olc::PixelGameEngine
{
public:
	Renderer* renderer;

	Object* player;

public:
	void MovePlayer(float fElapsedTime)
	{
		float speed = 10.0f;

		float x = 0;
		float y = 0;

		if (GetKey(olc::A).bHeld) {
			x -= speed * fElapsedTime;
		}
		if (GetKey(olc::D).bHeld) {
			x += speed * fElapsedTime;
		}

		if (GetKey(olc::W).bHeld) {
			y -= speed * fElapsedTime;
		}
		if (GetKey(olc::S).bHeld) {
			y += speed * fElapsedTime;
		}

		// Basic world collision here
		olc::vf2d playerPos = player->GetPosition();
		playerPos += {x, y};

		if (renderer->worldData->GetTile(playerPos.x, playerPos.y).ID != 0)
		{
			// Collide. This is not the intended behaviour but works for now.
			x = 0;
			y = 0;
		}

		player->velocity = { x,y };
		player->Update(fElapsedTime);
	}

public:
	MiniMinecraft()
	{
		// Name your application
		sAppName = "Mini minecraft";
	}

public:
	bool OnUserCreate() override
	{
		// Initialize the renderer
		renderer = new Renderer(this);

		// Player shit
		player = new Object(renderer->playerSpriteData);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Movement code
		MovePlayer(fElapsedTime);

		renderer->UpdateCameraPosition(player->GetPosition(), fElapsedTime);

		// Draw routine
		renderer->DrawWorld();
		renderer->DrawObject(player);
		renderer->UpdateSun(fElapsedTime);
		renderer->UpdateLights();

		return GetKey(olc::ESCAPE).bReleased == false;
	}
};

int main()
{
	// Hide console
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	// Show game
	MiniMinecraft demo;
	if (demo.Construct(264, 216, 4, 4))
		demo.Start();

	if (CRASH) {
		ShowWindow(GetConsoleWindow(), SW_SHOW);

		while (1) {}
	}

	return 0;
}