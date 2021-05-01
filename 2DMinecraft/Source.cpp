bool DEBUG = false;

#define OLC_PGE_APPLICATION
#include "Headers/olcPixelGameEngine.h"
#include "Headers/olcPGEX_Network.h"

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
	int tileId;

	// DEBUG REMOVE ME
	Tile* currentlySelectedTile;

	olc::Sprite* miniMapSprite;
	olc::Decal* miniMapDecal;

	uint8_t frameCount;

	bool drawMiniMap = false;
	byte miniMapDrawLocation = 1;
	float miniMapDrawScale = 0.25f;
	olc::Pixel* mapColors;

public:
	void MovePlayer(float fElapsedTime)
	{
		float speed = 50.0f;

		float x = 0;
		float y = 0;

		if (GetKey(olc::A).bHeld || GetKey(olc::LEFT).bHeld) {
			x -= speed * fElapsedTime;
		}
		if (GetKey(olc::D).bHeld || GetKey(olc::RIGHT).bHeld) {
			x += speed * fElapsedTime;
		}

		if (GetKey(olc::W).bHeld || GetKey(olc::UP).bHeld) {
			y -= speed * fElapsedTime;
		}
		if (GetKey(olc::S).bHeld || GetKey(olc::DOWN).bHeld) {
			y += speed * fElapsedTime;
		}

		// Basic world collision here
		olc::vf2d playerPos = player->GetPosition();
		playerPos += {x, y};

		currentlySelectedTile = renderer->worldData->GetTilePtr(playerPos.x, playerPos.y);

		if (currentlySelectedTile != nullptr)
			tileId = currentlySelectedTile->ID;

		// in water
		player->inWater = (tileId == 0 || tileId == 1);

		if (tileId != 0)
		{
			// FIXME: do things here
			// There is a tile where the player is trying to go!
		}

		player->velocity = { x,y };
		player->Update(fElapsedTime);
	}

	void UpdateMiniMap()
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < MAP_WIDTH; x++)
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				int dat = renderer->worldData->GetTileID(x, y);

				if (y * SPRITE_SCALE > player->GetPosition().y - 25 &&
					y * SPRITE_SCALE < player->GetPosition().y + 25 &&
					x * SPRITE_SCALE > player->GetPosition().x - 25 &&
					x * SPRITE_SCALE < player->GetPosition().x + 25)
				{
					miniMapSprite->SetPixel(x, y, olc::Pixel(255, 0, 0, 255));
				}
				else
				{
					miniMapSprite->SetPixel(x, y, mapColors[dat]);
				}
			}

		miniMapDecal->Update();
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

		miniMapSprite = new olc::Sprite(MAP_WIDTH, MAP_HEIGHT);
		miniMapDecal = new olc::Decal(miniMapSprite);

		mapColors = new olc::Pixel[WORLD_TILES_WIDTH * WORLD_TILES_HEIGHT];

		for (uint8_t i = 0; i < WORLD_TILES_WIDTH * WORLD_TILES_HEIGHT; i++)
		{
			int x = i % WORLD_TILES_WIDTH;
			int y = i / WORLD_TILES_WIDTH;

			x *= SPRITE_SCALE;
			y *= SPRITE_SCALE;

			mapColors[i] = renderer->tileSpriteData->Sprite()->GetPixel(x, y);
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		frameCount++;
		frameCount %= 255;

		// Movement code
		MovePlayer(fElapsedTime);

		renderer->UpdateCameraPosition(player->GetPosition(), fElapsedTime);

		// Draw routine
		renderer->DrawWorld();
		renderer->DrawObject(player);
		renderer->UpdateSun(fElapsedTime);
		renderer->UpdateLights();

		if (GetKey(olc::Key::M).bReleased)
		{
			drawMiniMap = !drawMiniMap;

			if (drawMiniMap == true)
			{
				UpdateMiniMap();
			}
		}

		if (GetKey(olc::Key::F3).bReleased)
		{
			DEBUG = !DEBUG;
		}

		// Draw ye bloody minimap
		if (drawMiniMap == true)
		{
			// L is for location
			if (GetKey(olc::Key::L).bReleased)
			{
				miniMapDrawLocation++;
				miniMapDrawLocation %= 4;
			}

			// I is for in
			if (GetKey(olc::Key::I).bHeld)
			{
				miniMapDrawScale += 0.01f;
				if (miniMapDrawScale > 1) miniMapDrawScale = 1;
			}

			// O is for out
			if (GetKey(olc::Key::O).bHeld)
			{
				miniMapDrawScale -= 0.01f;
				if (miniMapDrawScale < 0.05f) miniMapDrawScale = 0.05f;
			}

			// Only update the minimap every 'n'th frame
			if (frameCount % 20 == 0)
				UpdateMiniMap();

			// Draw the minimap

			olc::vf2d drawLocation;
			switch (miniMapDrawLocation)
			{
			case 0:
				drawLocation = { 0,0 };
				break;
			case 1:
				drawLocation = { ScreenWidth() - miniMapSprite->width * miniMapDrawScale, 0 };
				break;
			case 2:
				drawLocation = { ScreenWidth() - miniMapSprite->width * miniMapDrawScale, ScreenHeight() - miniMapSprite->height * miniMapDrawScale };
				break;
			case 3:
				drawLocation = { 0, ScreenHeight() - miniMapSprite->height * miniMapDrawScale };
				break;
			default:
				break;
			}

			DrawDecal(drawLocation, miniMapDecal, { miniMapDrawScale, miniMapDrawScale });
		}

		float xLoop = player->GetPosition().x;
		float yLoop = player->GetPosition().y;
		if (yLoop > MAP_HEIGHT * SPRITE_SCALE)
		{
			yLoop = 0;
		}

		if (xLoop > MAP_WIDTH * SPRITE_SCALE)
		{
			xLoop = 0;
		}

		if (yLoop < 0)
		{
			yLoop = MAP_HEIGHT * SPRITE_SCALE;
		}

		if (xLoop < 0)
		{
			xLoop = MAP_WIDTH * SPRITE_SCALE;
		}

		if (yLoop != player->GetPosition().y || xLoop != player->GetPosition().x)
		{
			player->SetPosition({ xLoop, yLoop });

			// This is gonna look like shit
			renderer->SnapCamera(player->GetPosition());
		}

		if (DEBUG)
		{
			DrawStringDecal({ 0,0 }, "pos:" + std::to_string(player->GetPosition().x) + ", " + std::to_string(player->GetPosition().y) + "\n" +
				"tileID:" + std::to_string(tileId),
				olc::YELLOW, { 0.5f, 0.5f });

			if (currentlySelectedTile != nullptr)
			{
				DrawDecal({ float(currentlySelectedTile->x * SPRITE_SCALE), float(currentlySelectedTile->y * SPRITE_SCALE) },
					renderer->whiteSquareDecal, { 1,1 }, olc::RED);
			}
		}

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

	if (DEBUG) {
		ShowWindow(GetConsoleWindow(), SW_SHOW);

		while (1) {}
	}

	return 0;
}