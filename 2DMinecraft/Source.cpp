bool CRASH = false;

#define OLC_PGE_APPLICATION
#include "Headers/olcPixelGameEngine.h"

const int8_t SPRITE_SCALE = 12;
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
		float speed = 50.0f;

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

		int tileId = renderer->worldData->GetTile(playerPos.x, playerPos.y).ID;

		if (tileId != 0)
		{
			// FIXME: do things here
			// There is a tile where the player is trying to go!
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
		player->SetPosition(renderer->worldData->GetRandomGroundTile());

		renderer->SnapCamera(player->GetPosition());

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

		DrawStringDecal({ 0,0 }, std::to_string(player->GetPosition().x) + ", " + std::to_string(player->GetPosition().y));

		return GetKey(olc::ESCAPE).bReleased == false;
	}
};

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name your application
		sAppName = "Example";
	}

	WorldData* worldData;

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		worldData = new WorldData();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				int dat = worldData->GetTileID(x, y);

				Draw(x, y, olc::Pixel(21 * dat, 21 * dat, 21 * dat));
			}
		return true;
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