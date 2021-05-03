bool DEBUG = false;
bool APPLICATION_RUNNING = true;
bool IS_SERVER = false;

#define OLC_PGE_APPLICATION
#include "Headers/olcPixelGameEngine.h"

const int8_t SPRITE_SCALE = 12;

#define OLC_PGEX_NETWORK
#include "Headers/olcPGEX_Network.h"
#include "Headers/networkCommon.h"

#include "Headers/uiObjects.h"
#include "Headers/debugger.h"

#include "Headers/worldData.h"
#include "Headers/objectDefinitions.h"
#include "Headers/renderer.h"

#include "Headers/sceneDefinition.h"

#include <unordered_map>

// Override base class with your custom functionality
class MiniMinecraft : public olc::PixelGameEngine
{
public:
	MiniMinecraft()
	{
		// Name your application
		sAppName = "Mini minecraft";
	}

public:

	TimeConstruct* time;

	IntroScene* introScene = nullptr;
	MainMenu* mainMenu = nullptr;
	MultiplayerLobby* multiplayerLobby = nullptr;
	SinglePlayer* singlePlayer = nullptr;
	MultiPlayerClient* multiPlayer = nullptr;

public:
	void HandleScenes()
	{
		switch (currentScene)
		{
		case SCENE::SCENE_INTRO:
			if (introScene == nullptr)
			{
				introScene = new IntroScene();
				introScene->Initialize(time, this);
			}

			introScene->UpdateScene();
			break;
		case SCENE::SCENE_MAIN_MENU:
			if (introScene != nullptr)
			{
				delete introScene;
				introScene = nullptr;
			}

			if (singlePlayer != nullptr)
			{
				delete singlePlayer;
				singlePlayer = nullptr;
			}

			if (mainMenu == nullptr)
			{
				mainMenu = new MainMenu();
				mainMenu->Initialize(time, this);
			}

			mainMenu->UpdateScene();

			break;

		case SCENE::SCENE_MP_LOBBY:
			if (mainMenu != nullptr)
			{
				delete mainMenu;
				mainMenu = nullptr;
			}

			if (multiplayerLobby == nullptr)
			{
				multiplayerLobby = new MultiplayerLobby();
				multiplayerLobby->Initialize(time, this);
			}

			multiplayerLobby->UpdateScene();

			break;

		case SCENE::SCENE_SINGLE_PLAYER:
			if (mainMenu != nullptr)
			{
				delete mainMenu;
				mainMenu = nullptr;
			}

			if (singlePlayer == nullptr)
			{
				singlePlayer = new SinglePlayer();
				singlePlayer->Initialize(time, this);
			}

			singlePlayer->UpdateScene();

			break;

		case SCENE::SCENE_MULTI_PLAYER:
			if (mainMenu != nullptr)
			{
				delete mainMenu;
				mainMenu = nullptr;
			}

			if (multiPlayer == nullptr)
			{
				multiPlayer = new MultiPlayerClient();
				multiPlayer->Initialize(time, this);
			}

			multiPlayer->UpdateScene();

			break;

		default:
			break;
		}
	}

public:
	bool OnUserCreate() override
	{
		time = new TimeConstruct();
		return !DEBUG;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		time->Update(fElapsedTime);
		HandleScenes();

		return APPLICATION_RUNNING;
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

	if (DEBUG && debug_prints > 0) {
		ShowWindow(GetConsoleWindow(), SW_SHOW);

		while (1) {}
	}

	return 0;
}