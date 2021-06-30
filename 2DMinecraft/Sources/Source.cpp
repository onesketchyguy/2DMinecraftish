// Forrest Lowe 2021

//#define DEBUG_BUILD

#define OLC_PGE_APPLICATION
#include "../resource.h"
#include "../Headers/olcPixelGameEngine.h"

#include "../Headers/ConstantData.h"

bool DEBUG = false;
bool APPLICATION_RUNNING = true;

const int8_t SPRITE_SCALE = 12;
olc::vi2d spriteScale = { (int)SPRITE_SCALE, (int)SPRITE_SCALE };

const uint8_t WORLD_TILES_WIDTH = 4;
const uint8_t WORLD_TILES_HEIGHT = 3;

const uint8_t WORLD_ITEMS_WIDTH = 3;
const uint8_t WORLD_ITEMS_HEIGHT = 1;

const uint8_t WATER_SPRITE_INDEX = 3;

std::string worldSeed = "";

std::string serverIP = "127.0.0.1";
const int SERVER_PORT = 60000;

#include "../Headers/uiObjects.h"
#include "../Headers/debugger.h"

#include "../Headers/introScene.h"
#include "../Headers/mainMenu.h"
#include "../Headers/gameScene.h"

// Override base class with your custom functionality
class MiniMinecraft : public olc::PixelGameEngine
{
public:
	MiniMinecraft()
	{
		// Name your application
		sAppName = "Mini minecraft";
	}

	~MiniMinecraft()
	{
		delete introScene;
		delete mainMenu;
		delete gameScene;
		delete renderer;
		delete time;
	}

public:

	TimeConstruct* time = nullptr;
	Renderer* renderer = nullptr;

	IntroScene* introScene = nullptr;
	MainMenu* mainMenu = nullptr;
	GameScene* gameScene = nullptr;

public:
	void HandleScenes()
	{
		switch (currentScene)
		{
		case SCENE::SCENE_INTRO:
			if (introScene == nullptr)
			{
				print("Loading intro scene");

				introScene = new IntroScene();
				introScene->Initialize(time, this, renderer);
			}
			else
			{
				introScene->UpdateScene();
			}

			break;
		case SCENE::SCENE_MAIN_MENU:
			if (mainMenu == nullptr)
			{
				print("Loading main menu scene");

				mainMenu = new MainMenu();
				mainMenu->Initialize(time, this, renderer);
			}
			else
			{
				mainMenu->UpdateScene();
			}

			// Cleanup old scenes
			if (introScene != nullptr)
			{
				delete introScene;
				introScene = nullptr;
			}

			if (gameScene != nullptr)
			{
				delete gameScene;
				gameScene = nullptr;
			}

			break;

		case SCENE::SCENE_GAME:
			if (gameScene == nullptr)
			{
				renderer->ClearWorldData();

				print("Loading game scene");

				gameScene = new GameScene();
				gameScene->Initialize(time, this, renderer);
			}
			else
			{
				gameScene->UpdateScene();
			}

			// Cleanup old scenes
			if (mainMenu != nullptr)
			{
				delete mainMenu;
				mainMenu = nullptr;
			}

			break;

		default:
			break;
		}
	}

public:
	bool OnUserCreate() override
	{
		time = new TimeConstruct();
		renderer = new Renderer(this);

		return !DEBUG;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::F3).bReleased)
		{
			DEBUG = !DEBUG;

			if (DEBUG)
			{
				ShowWindow(GetConsoleWindow(), SW_SHOW);
			}
			else
			{
				ShowWindow(GetConsoleWindow(), SW_HIDE);
			}
		}

		time->Update(fElapsedTime);
		HandleScenes();

		return APPLICATION_RUNNING;
	}
};

int main()
{
	// Hide console
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	clearDebugFile();
	print("_____Session started____");

	// Show game
	MiniMinecraft demo;
	if (demo.Construct(364, 216, 3, 3))
		demo.Start();

	if (DEBUG && debug_prints > 0)
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);

		while (1);
	}

	return 0;
}