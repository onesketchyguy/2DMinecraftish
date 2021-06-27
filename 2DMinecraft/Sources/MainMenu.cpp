#include "../Headers/MainMenu.h"

void MainMenu::HandleMenus()
{
	switch (currentMenu)
	{
	case MAIN_SUBMENU::MENU_MAIN:
		title->Draw();

		// Singleplayer button
		singlePlayerButton->Draw();

		if (singlePlayerButton->MouseOver())
		{
			singlePlayerButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				currentMenu = MAIN_SUBMENU::MENU_SPLOBBY;
				serverIpText->SetString("SEED: " + worldSeed);
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
		if (typing)
		{
			RecieveNumericalInput(serverIP);
			// Update the string when needed

			// FIXME: this is fine
			char cursor = cursorVisable ? '_' : ' ';

			serverIpText->SetString(serverIP + cursor);
		}

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

	case MAIN_SUBMENU::MENU_SPLOBBY: // FIXME: do things correctly
		hostButton->SetString("Play");
		hostButton->Draw();
		serverIpText->Draw();
		backButton->Draw();

		// Host button
		if (hostButton->MouseOver())
		{
			hostButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				currentScene = SCENE::SCENE_GAME;
				playMode = PLAY_MODE::SINGLE_PLAYER;
			}
		}
		else hostButton->fillColor = defaultColor;

		// Ip text
		if (typing)
		{
			RecieveStringInput(worldSeed);
			// Update the string when needed

			// FIXME: this is fine
			char cursor = cursorVisable ? '_' : ' ';

			serverIpText->SetString("SEED: " + worldSeed + cursor);
		}

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
					serverIpText->SetString("SEED: " + worldSeed);
				}
			}
			else serverIpText->fillColor = defaultColor;
		}

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

	singlePlayerButton = new TextBox(engine, "single-player",
		xPos, yPos,
		125, 25,
		1, 1,
		10, 10,
		olc::BLACK, defaultColor);

	yPos += 40;
#ifdef DEBUG_BUILD
	multiPlayerButton = new TextBox(engine, "multi-player",
		xPos, yPos,
		125, 25,
		1, 1,
		10, 10,
		olc::BLACK, defaultColor);

	xPos += 25;
	yPos += 40;
#endif
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

	worldData = new WorldData();
	renderer = new Renderer(engine, worldData);

	return true;
}

bool MainMenu::Update()
{
	if (worldData->GetWorldGenerated() == false)
	{
		if (worldData->GetWorldGenerating() == false)
		{
			worldData->GenerateMap();
		}

		backGroundImagePos = worldData->GetRandomGroundTile();
		backGroundImageTarget = worldData->GetRandomGroundTile();

		return true;
	}

	// Draw the menu

	olc::vf2d backGroundImageDir = (backGroundImageTarget - backGroundImagePos) * 0.005f;

	backGroundImagePos.x += backGroundImageDir.x * time->elapsedTime;
	backGroundImagePos.y += backGroundImageDir.y * time->elapsedTime;

	if (time->frameCount % 200 == 0 || abs(backGroundImageDir.mag()) < 0.001f)
	{
		backGroundImageTarget = worldData->GetRandomGroundTile();
	}

	renderer->SetCamera(backGroundImagePos);
	renderer->DrawWorld();
	renderer->SetZoomScaleToMin();

	HandleMenus();
	return true;
}