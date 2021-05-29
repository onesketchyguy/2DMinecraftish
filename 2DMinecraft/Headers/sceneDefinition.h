// Forrest Lowe 2021

#pragma once
#ifndef SCENE_DEFINITIONS_H
#define SCENE_DEFINITIONS_H

//#define DEBUG_BUILD

#define OLC_PGEX_NETWORK
#include "olcPGEX_Network.h"
#include "networkCommon.h"

#include "ConstantData.h"

#include "timeConstruct.h"

#include "worldData.h"
#include "miniMap.h"

#include "objectDefinitions.h"
#include "renderer.h"

#include <unordered_map>

/* example of use
class MainMenu : public Scene
{
public:
	bool OnLoad() override
	{
		return true;
	}

	bool Update() override
	{
		return true;
	}
};*/

enum class PLAY_MODE : uint8_t
{
	SINGLE_PLAYER,
	CLIENT,
	SERVER
};

PLAY_MODE playMode = PLAY_MODE::SINGLE_PLAYER;

enum class SCENE : uint8_t
{
	SCENE_INTRO,
	SCENE_MAIN_MENU,
	SCENE_GAME
};

SCENE currentScene = SCENE::SCENE_INTRO;

class Scene
{
public:
	void Initialize(TimeConstruct* time, olc::PixelGameEngine* engine)
	{
		this->time = time;
		this->engine = engine;
	}

protected:
	olc::PixelGameEngine* engine;

	TimeConstruct* time;

	olc::HWButton GetKey(olc::Key key)
	{
		return engine->GetKey(key);
	}

	olc::HWButton GetMouse(uint32_t button)
	{
		return engine->GetMouse(button);
	}

	int32_t ScreenWidth()
	{
		return engine->ScreenWidth();
	}

	int32_t ScreenHeight()
	{
		return engine->ScreenHeight();
	}

private:
	bool sceneLoaded = false;

public:
	bool UpdateScene()
	{
		if (!sceneLoaded)
		{
			sceneLoaded = OnLoad();
		}
		else
		{
			sceneLoaded = Update();
		}

		return sceneLoaded;
	}

	virtual bool OnLoad() { return true; }
	virtual bool Update() { return true; }
};

class IntroScene : public Scene
{
	olc::Renderable* olc_logo;
	olc::Renderable* flowe_logo;
	olc::vf2d logoPos;
	olc::vf2d olc_logoSize = { 0.25f, 0.25f };
	olc::vf2d flowe_logoSize = { 1.25f, 1.25f };

	float logoTime = 3;

public:
	~IntroScene()
	{
		delete olc_logo;
		delete flowe_logo;
	}

public:
	bool OnLoad() override
	{
		logoPos.x = ScreenWidth() / 2;
		logoPos.y = ScreenHeight() / 2;

		olc_logo = new olc::Renderable();
		olc_logo->Load("Data/olc_logo.png");

		flowe_logo = new olc::Renderable();
		flowe_logo->Load("Data/flowe_logo.png");

		logoPos.x -= olc_logo->Sprite()->width * (olc_logoSize.x / 2.0f);
		logoPos.y -= olc_logo->Sprite()->height * (olc_logoSize.y / 2.0f);

		return true;
	}

	bool Update() override
	{
		engine->Clear(olc::DARK_BLUE);

		engine->DrawDecal({ logoPos.x, logoPos.y - 50 }, olc_logo->Decal(), olc_logoSize);
		engine->DrawDecal({ logoPos.x, logoPos.y + 50 }, flowe_logo->Decal(), flowe_logoSize);

		logoTime -= time->elapsedTime;
		if (logoTime <= 0)
		{
			currentScene = SCENE::SCENE_MAIN_MENU;
		}

		return true;
	}
};

class MainMenu : public Scene
{
private:
	TextBox* title;

	TextBox* singlePlayerButton;
	TextBox* multiPlayerButton;
	TextBox* quitButton;

	TextBox* hostButton;
	TextBox* serverIpText;
	TextBox* joinButton;
	TextBox* backButton;

	bool typing = false;
	bool cursorVisable = true;
	const float CURSOR_FLASH_TIME = 0.25f;
	float flashTime = 0;

	olc::Pixel defaultColor = olc::WHITE;
	olc::Pixel highlightColor = olc::GREY;

	enum class MAIN_SUBMENU : uint8_t
	{
		MENU_MAIN,
		MENU_MPLOBBY
	};

	MAIN_SUBMENU currentMenu;

public:
	~MainMenu()
	{
		delete title;
		delete singlePlayerButton;
		delete multiPlayerButton;
		delete quitButton;
		delete hostButton;
		delete joinButton;
		delete backButton;
	}

private:
	void RecieveIpInput()
	{
		// Backspace
		if (GetKey(olc::Key::BACK).bReleased && serverIP.length() > 0)
		{
			serverIP.pop_back();
		}

		// Numpad numbers
		for (size_t i = 69; i < 79; i++)
		{
			if (GetKey((olc::Key)i).bReleased)
			{
				serverIP += char(i + 48 - 69);
			}
		}

		// Keyboard numbers
		for (size_t i = 27; i < 37; i++)
		{
			if (GetKey((olc::Key)i).bReleased)
			{
				serverIP += char(i + 48 - 27);
			}
		}

		// Decimal place
		if (GetKey(olc::Key::PERIOD).bReleased || GetKey(olc::Key::NP_DECIMAL).bReleased)
		{
			serverIP += '.';
		}

		// Flash the cursor on screen
		flashTime += time->elapsedTime;

		if (flashTime > CURSOR_FLASH_TIME)
		{
			cursorVisable = !cursorVisable;
			flashTime = 0;
		}

		char cursor = cursorVisable ? '_' : ' ';

		// Update the string when needed
		serverIpText->SetString(serverIP + cursor);
	}

	void HandleMenus()
	{
		switch (currentMenu)
		{
		case MAIN_SUBMENU::MENU_MAIN:
			if (time->frameCount % 10 == 0)
			{
				// Reseed the world generator
				srand(static_cast<unsigned int>(std::time(0)));
			}

			title->Draw();

			// Singleplayer button
			singlePlayerButton->Draw();

			if (singlePlayerButton->MouseOver())
			{
				singlePlayerButton->fillColor = highlightColor;

				if (engine->GetMouse(0).bReleased)
				{
					currentScene = SCENE::SCENE_GAME;
					playMode = PLAY_MODE::SINGLE_PLAYER;
				}
			}
			else singlePlayerButton->fillColor = defaultColor;

#ifdef DEBUG_BUILD
			// Multiplayer button
			multiPlayerButton->Draw();

			if (multiPlayerButton->MouseOver())
			{
				multiPlayerButton->fillColor = highlightColor;

				if (engine->GetMouse(0).bReleased)
				{
					currentMenu = MAIN_SUBMENU::MENU_MPLOBBY;
				}
			}
			else multiPlayerButton->fillColor = defaultColor;
#endif

			// Quit button
			quitButton->Draw();

			if (quitButton->MouseOver())
			{
				quitButton->fillColor = highlightColor;

				if (engine->GetMouse(0).bReleased)
				{
					APPLICATION_RUNNING = false;
				}
			}
			else quitButton->fillColor = defaultColor;

			break;
		case MAIN_SUBMENU::MENU_MPLOBBY:
			hostButton->Draw();
			joinButton->Draw();
			serverIpText->Draw();
			backButton->Draw();

			// Host button
			if (hostButton->MouseOver())
			{
				hostButton->fillColor = highlightColor;

				if (engine->GetMouse(0).bReleased)
				{
					currentScene = SCENE::SCENE_GAME;
					playMode = PLAY_MODE::SERVER;
				}
			}
			else hostButton->fillColor = defaultColor;

			// Ip text
			if (typing) RecieveIpInput();
			if (serverIpText->MouseOver())
			{
				serverIpText->fillColor = highlightColor;

				if (engine->GetMouse(0).bReleased)
				{
					typing = true;
				}
			}
			else
			{
				if (typing)
				{
					if (engine->GetMouse(0).bReleased)
					{
						typing = false;
						serverIpText->SetString(serverIP);
					}
				}
				else serverIpText->fillColor = defaultColor;
			}

			// Join button
			if (joinButton->MouseOver())
			{
				joinButton->fillColor = highlightColor;

				if (engine->GetMouse(0).bReleased)
				{
					currentScene = SCENE::SCENE_GAME;
					playMode = PLAY_MODE::CLIENT;
				}
			}
			else joinButton->fillColor = defaultColor;

			// Back button
			if (backButton->MouseOver())
			{
				backButton->fillColor = highlightColor;

				if (engine->GetMouse(0).bReleased)
				{
					currentMenu = MAIN_SUBMENU::MENU_MAIN;
				}
			}
			else backButton->fillColor = defaultColor;
			break;
		default:
			break;
		}
	}

public:
	bool OnLoad() override
	{
		int xPos = (ScreenWidth() / 2) - 75;
		int yPos = (ScreenHeight() / 2) - 50;

		title = new TextBox(engine, "Mini-craft",
			xPos, yPos,
			100, 25,
			2, 2,
			0, 0,
			olc::WHITE);

		xPos += 25;
		yPos += 40;

		singlePlayerButton = new TextBox(engine, "single player",
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		yPos += 40;

		multiPlayerButton = new TextBox(engine, "multi player",
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		xPos += 25;
		yPos += 40;

		quitButton = new TextBox(engine, "quit",
			xPos, yPos,
			60, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		xPos = (ScreenWidth() / 2) - 75;
		yPos = (ScreenHeight() / 2) - 50;

		hostButton = new TextBox(engine, "host game",
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		yPos += 40;

		serverIpText = new TextBox(engine, serverIP,
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		yPos += 40;

		joinButton = new TextBox(engine, "join game",
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		xPos += 25;
		yPos += 40;

		backButton = new TextBox(engine, "back",
			xPos, yPos,
			60, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		return true;
	}

	bool Update() override
	{
		// Draw the menu
		engine->Clear(olc::DARK_BLUE);

		HandleMenus();
		return true;
	}
};

#endif // !SCENE_DEFINITIONS_H