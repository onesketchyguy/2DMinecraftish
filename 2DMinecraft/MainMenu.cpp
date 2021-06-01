#include "Headers/MainMenu.h"

void MainMenu::RecieveIpInput()
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

void MainMenu::HandleMenus()
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

bool MainMenu::OnLoad()
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

	backGround = new olc::Renderable();
	backGround->Load("Data/mm.png");

	return true;
}

bool MainMenu::Update()
{
	// Draw the menu

	backGroundImagePos.x += backGroundImageDir.x * time->elapsedTime;
	backGroundImagePos.y += backGroundImageDir.y * time->elapsedTime;

	if (backGroundImagePos.x + ScreenWidth() / 2 >= ScreenWidth() - 1)
	{
		backGroundImageDir.x = -backGroundImageDir.x;
	}

	if (backGroundImagePos.x <= 1)
	{
		backGroundImageDir.x = -backGroundImageDir.x;
	}

	if (backGroundImagePos.y + ScreenHeight() / 2 >= ScreenHeight() - 1)
	{
		backGroundImageDir.y = -backGroundImageDir.y;
	}

	if (backGroundImagePos.y <= 1)
	{
		backGroundImageDir.y = -backGroundImageDir.y;
	}

	DrawPartialDecal({ 0,0 }, olc::vi2d{ ScreenWidth(), ScreenHeight() },
		backGround->Decal(), backGroundImagePos,
		olc::vi2d{ ScreenWidth(), ScreenHeight() });

	HandleMenus();
	return true;
}