bool DEBUG = false;

#define OLC_PGE_APPLICATION
#include "Headers/olcPixelGameEngine.h"
//#include "Headers/olcPGEX_Network.h"
//#include "Headers/networkCommon.h"

const int8_t SPRITE_SCALE = 12;
#include "Headers/worldData.h"
#include "Headers/objectDefinitions.h"
#include "Headers/renderer.h"

//#include <unordered_map>

// Override base class with your custom functionality
class MiniMinecraft : public olc::PixelGameEngine//, olc::net::client_interface<GameMsg>
{
public:
	Renderer* renderer;
	WorldData* worldData;

	Object* localPlayer;
	int tileId;
	int foliageID;

	olc::Sprite* miniMapSprite;
	olc::Decal* miniMapDecal;

	uint8_t frameCount;

	bool drawMiniMap = false;
	byte miniMapDrawLocation = 1;
	float miniMapDrawScale = 0.25f;
	olc::Pixel* mapColors;

private:
	//std::unordered_map<uint32_t, PlayerDescription> mapObjects;
	//uint32_t playerID = 0;
	//PlayerDescription descPlayer;

	//bool waitingForConnection = true;

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
		olc::vf2d playerPos = localPlayer->GetPosition();
		playerPos += {x, y};

		tileId = worldData->GetTileID(playerPos.x, playerPos.y);

		// in water
		localPlayer->inWater = (tileId == 0 || tileId == 1);

		if (tileId != 0)
		{
			// FIXME: do things here
			// There is a tile where the player is trying to go!
		}
		//else {
		//	x = 0;
		//	y = 0;
		//}

		localPlayer->velocity = { x,y };
		localPlayer->Update(fElapsedTime);
	}

	/// <summary>
	/// Loops the player around the world.
	/// </summary>
	void LoopPlayer()
	{
		float xLoop = localPlayer->GetPosition().x;
		float yLoop = localPlayer->GetPosition().y;
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

		if (yLoop != localPlayer->GetPosition().y || xLoop != localPlayer->GetPosition().x)
		{
			olc::vf2d targetCameraPosition = localPlayer->GetPosition() - ANIMATION::spriteScale * 5;
			olc::vf2d cameraOffset = targetCameraPosition - renderer->cameraPosition;

			localPlayer->SetPosition({ xLoop, yLoop });

			// This is gonna look like shit
			renderer->SnapCamera(localPlayer->GetPosition() - cameraOffset);
		}
	}

	//void MovePlayers()
	//{
	//	for (auto& object : mapObjects)
	//	{
	//		object.second.position += object.second.velocity;
	//	}
	//}

	void UpdateMiniMap()
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < MAP_WIDTH; x++)
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				int dat = renderer->worldData->GetTileID(x, y);

				if (y * SPRITE_SCALE > localPlayer->GetPosition().y - 25 &&
					y * SPRITE_SCALE < localPlayer->GetPosition().y + 25 &&
					x * SPRITE_SCALE > localPlayer->GetPosition().x - 25 &&
					x * SPRITE_SCALE < localPlayer->GetPosition().x + 25)
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
		worldData = new WorldData();

		// Initialize the renderer
		renderer = new Renderer(this, worldData);

		// Player shit
		localPlayer = new Object(renderer->playerSpriteData);
		localPlayer->SetPosition(renderer->worldData->GetRandomGroundTile());

		renderer->SnapCamera(localPlayer->GetPosition());

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

		//if (Connect("127.0.0.1", 60000))
		//{
		//	return true;
		//}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		frameCount++;
		frameCount %= 255;

		//if (waitingForConnection)
		//{
		//	Clear(olc::DARK_BLUE);
		//	DrawString({ 10,10 }, "Waiting To Connect...", olc::WHITE);
		//	return true;
		//}

		// Movement code
		MovePlayer(fElapsedTime);
		LoopPlayer();

		// Set the action direction
		olc::vf2d lookDirection = { 0,0 };
		switch (localPlayer->GetLookDir())
		{
		case ANIMATION::right:
			lookDirection.x += SPRITE_SCALE;
			break;
		case ANIMATION::left:
			lookDirection.x -= SPRITE_SCALE;
			break;
		case ANIMATION::down:
			lookDirection.y += SPRITE_SCALE;
			break;
		case ANIMATION::up:
			lookDirection.y -= SPRITE_SCALE;
			break;
		default:
			break;
		}

		lookDirection += localPlayer->GetPosition();

		renderer->UpdateCameraPosition(localPlayer->GetPosition(), fElapsedTime);

		// Draw routine
		renderer->DrawWorld();
		renderer->DrawObject(localPlayer);
		renderer->UpdateSun(fElapsedTime);
		renderer->UpdateLights();

		// Debug, move this later
		foliageID = worldData->GetFoliageID(lookDirection.x, lookDirection.y);

		if (GetKey(olc::SPACE).bPressed)
		{
			// Do some shit :D

			// Do the action
			if (foliageID == 2) // Hit tree
			{
				worldData->SetFoliageID(lookDirection.x, lookDirection.y, 0);
			}

			if (DEBUG)
			{
				std::cout << "Player tried to hit: " << foliageID << " while standing at:" << lookDirection.x << "," << lookDirection.y << std::endl;
			}
		}

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

		if (DEBUG)
		{
			DrawStringDecal({ 0,0 }, "pos:" + std::to_string(localPlayer->GetPosition().x) + ", " + std::to_string(localPlayer->GetPosition().y) + "\n" +
				"tileID:" + std::to_string(tileId) + "\n" +
				"foliageID:" + std::to_string(foliageID),
				olc::YELLOW, { 0.5f, 0.5f });

			renderer->DrawDecal(lookDirection, { 1,1 }, renderer->whiteSquareDecal);
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

	olc::vf2d mapPlayer;

	olc::Sprite* miniMapSprite;
	olc::Decal* miniMapDecal;

	Renderer* renderer;
	WorldData* worldData;
	olc::Pixel* mapColors;

	olc::vf2d mapScale;

	void UpdateMiniMap()
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < MAP_WIDTH; x++)
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				int dat = renderer->worldData->GetTileID(x, y);

				if (y * SPRITE_SCALE > mapPlayer.y - 25 &&
					y * SPRITE_SCALE < mapPlayer.y + 25 &&
					x * SPRITE_SCALE > mapPlayer.x - 25 &&
					x * SPRITE_SCALE < mapPlayer.x + 25)
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
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		worldData = new WorldData();
		renderer = new Renderer(this, worldData);

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

		mapScale = {
			float(ScreenWidth()) / float(miniMapSprite->width),
			float(ScreenHeight()) / float(miniMapSprite->height) };

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		UpdateMiniMap();
		DrawDecal({ 0,0 }, miniMapDecal, mapScale);

		return true;
	}
};

int main()
{
	// Hide console
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	// Show game
	MiniMinecraft demo;
	if (demo.Construct(264, 216, 3, 3))
		demo.Start();

	if (DEBUG) {
		ShowWindow(GetConsoleWindow(), SW_SHOW);

		while (1) {}
	}

	return 0;
}