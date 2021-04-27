#define OLC_PGE_APPLICATION
#include "Headers/olcPixelGameEngine.h"
#include "Headers/tileDefinitions.h"
#include "Headers/objectDefinitions.h"

bool CRASH = false;

// Override base class with your custom functionality
class MiniMinecraft : public olc::PixelGameEngine
{
public:
	Object* player;
	olc::Renderable* playerSpriteData;

	olc::Sprite* whiteSquareSprite;
	olc::Decal* whiteSquareDecal;

	olc::Pixel sunMap;
	float dayLight = 1;
	bool sunGoingDown = true;
	float sunSpeed = 0.1f;

	olc::Pixel* tileLightMap;
	LightSource* lightSources;
	uint8_t LIGHT_COUNT = 216;

	Tile* worldTiles;
	const uint8_t TILE_COUNT = 216;
	olc::Renderable* worldTileData;

	olc::vf2d cameraPosition = { 0.0f, 0.0f };
	olc::vf2d targetCameraPosition = { 0.0f, 0.0f };

	const float cameraSpeed = 3.5f;

public:
	int GetTile(float x, float y)
	{
		x /= ANIMATION::SPRITE_SCALE;
		y /= ANIMATION::SPRITE_SCALE;

		int X = int(ceil(x));
		int Y = int(ceil(y));

		int index = X * 9 + Y;
		return index;
	}

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

		int tileIndex = GetTile(playerPos.x, playerPos.y);
		if (worldTiles[tileIndex].ID != 0)
		{
			// Collide. This is not the intended behaviour but works for now.
			x = 0;
			y = 0;
		}

		player->velocity = { x,y };
		player->Update(fElapsedTime);
	}

	void DrawWorld()
	{
		SetPixelMode(olc::Pixel::NORMAL);

		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			int cellIndex_x = worldTiles[i].ID;
			int cellIndex_y = 0;

			olc::vf2d pos = { float(worldTiles[i].x),  float(worldTiles[i].y) };
			pos *= ANIMATION::spriteScale;

			if (pos.y >= ScreenHeight() || pos.y < 0 || pos.x >= ScreenWidth() || pos.x < 0) continue;

			olc::vi2d spriteCell = { ANIMATION::spriteScale.x * cellIndex_x, ANIMATION::spriteScale.y * cellIndex_y };
			DrawPartialDecal(pos - cameraPosition,
				ANIMATION::spriteScale,
				worldTileData->Decal(), spriteCell, ANIMATION::spriteScale);
		}
	}

	void UpdateLights()
	{
		SetPixelMode(olc::Pixel::MASK);

		// Update all the lights
		// FIXME: Put lights into world space
		for (uint8_t i = 0; i < LIGHT_COUNT; i++)
		{
			olc::Pixel color = lightSources[i].color;

			color.a = 255.0f * lightSources[i].intensity;
			tileLightMap[i] = color;
		}

		// Draw the lights
		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			olc::vf2d pos = { float(worldTiles[i].x),  float(worldTiles[i].y) };
			pos *= ANIMATION::spriteScale;

			if (pos.y >= ScreenHeight() || pos.y < 0 || pos.x >= ScreenWidth() || pos.x < 0) continue;

			DrawDecal(pos - cameraPosition, whiteSquareDecal, { 1.0, 1.0 },
				(tileLightMap[i] * (1 - dayLight)) + (sunMap * dayLight));
		}

		SetPixelMode(olc::Pixel::NORMAL);
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
		// Player shit
		playerSpriteData = new olc::Renderable();
		playerSpriteData->Load("Data/player.png");

		if (playerSpriteData->Decal() == nullptr) {
			std::cout << "Could not load player sprites!" << std::endl;

			CRASH = true;
			return false;
		}

		player = new Object(playerSpriteData, this);

		// World shit
		worldTileData = new olc::Renderable();
		worldTileData->Load("Data/WorldTiles.png");

		worldTiles = new Tile[TILE_COUNT];
		for (uint8_t i = 0; i < TILE_COUNT; i++)
		{
			worldTiles[i].x = i / 9;
			worldTiles[i].y = i % 9;
			//worldTiles[i].ID = rand() % 2;
			worldTiles[i].ID = 0;
		}

		worldTiles[0].ID = 0;

		// Lighting shit
		sunMap.a = 255;
		sunMap.r = 255;
		sunMap.g = 255;
		sunMap.b = 255;

		whiteSquareSprite = new olc::Sprite(ANIMATION::SPRITE_SCALE, ANIMATION::SPRITE_SCALE);

		for (uint8_t x = 0; x < ANIMATION::SPRITE_SCALE; x++)
		{
			for (uint8_t y = 0; y < ANIMATION::SPRITE_SCALE; y++)
			{
				whiteSquareSprite->SetPixel(x, y, olc::WHITE);
			}
		}

		whiteSquareDecal = new olc::Decal(whiteSquareSprite);

		tileLightMap = new olc::Pixel[TILE_COUNT];
		lightSources = new LightSource[LIGHT_COUNT];

		for (uint8_t i = 0; i < LIGHT_COUNT; i++)
		{
			tileLightMap[i] = olc::Pixel(rand() % 255, rand() % 255, rand() % 255);
			lightSources[i].Initialize({ 0.0f, 0.0f }, tileLightMap[i], 0.5f);
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Draw routine
		DrawWorld();
		player->Draw(cameraPosition);
		UpdateLights();

		if (sunGoingDown)
		{
			dayLight -= sunSpeed * fElapsedTime;

			if (dayLight <= 0)
			{
				sunGoingDown = false;
			}
		}
		else
		{
			dayLight += sunSpeed * fElapsedTime;

			if (dayLight >= 1)
			{
				sunGoingDown = true;
			}
		}

		// Movement code
		MovePlayer(fElapsedTime);

		// Camera movement
		targetCameraPosition = player->GetPosition() - ANIMATION::spriteScale * 5;
		olc::vf2d moveAmount = (targetCameraPosition - cameraPosition) * cameraSpeed * fElapsedTime;

		cameraPosition += moveAmount;

		return GetKey(olc::ESCAPE).bReleased == false;
	}
};

int main()
{
	// Hide console
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	// Show game
	MiniMinecraft demo;
	if (demo.Construct(264 / 2, 216 / 2, 4 * 2, 4 * 2))
		demo.Start();

	if (CRASH) {
		ShowWindow(GetConsoleWindow(), SW_SHOW);

		while (1) {}
	}

	return 0;
}