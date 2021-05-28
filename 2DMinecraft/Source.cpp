// Forrest Lowe 2021

bool DEBUG = false;
bool APPLICATION_RUNNING = true;
const int SERVER_PORT = 60000;

#define OLC_PGE_APPLICATION
#include "Headers/olcPixelGameEngine.h"

#include "Headers/uiObjects.h"
#include "Headers/debugger.h"

#include "Headers/sceneDefinition.h"

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
	GameScene* gameScene = nullptr;

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
			else
			{
				introScene->UpdateScene();
			}

			break;
		case SCENE::SCENE_MAIN_MENU:
			if (mainMenu == nullptr)
			{
				mainMenu = new MainMenu();
				mainMenu->Initialize(time, this);
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
				gameScene = new GameScene();
				gameScene->Initialize(time, this);
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

	print("_____Session started____");

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