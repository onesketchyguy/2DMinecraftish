#pragma once

#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "Headers/sceneDefinition.h"

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

	olc::Renderable* backGround;
	olc::vf2d backGroundImagePos{ 0.0f,0.0f };
	olc::vf2d backGroundImageDir{ 5.0f, 5.0f };

	bool typing = false;

	olc::Pixel defaultColor = olc::WHITE;
	olc::Pixel highlightColor = olc::GREY;

	enum class MAIN_SUBMENU : uint8_t
	{
		MENU_MAIN,
		MENU_MPLOBBY,
		MENU_SPLOBBY
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
		delete serverIpText;
		delete joinButton;
		delete backButton;
	}

private:
	void HandleMenus();
public:
	bool OnLoad();
	bool Update();
};

#endif // !MAIN_MENU_H
