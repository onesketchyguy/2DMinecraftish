// Forrest Lowe 2021

#include "../Headers/introScene.h"

bool IntroScene::OnLoad()
{
	logoPos.x = ScreenWidth() / 2.0f;
	logoPos.y = ScreenHeight() / 2.0f;

	olc_logo = new olc::Renderable();
	olc_logo->Load("Data/olc_logo.png");

	flowe_logo = new olc::Renderable();
	flowe_logo->Load("Data/flowe_logo.png");

	logoPos.x -= olc_logo->Sprite()->width * (olc_logoSize.x / 2.0f);
	logoPos.y -= olc_logo->Sprite()->height * (olc_logoSize.y / 2.0f);

	return true;
}

bool IntroScene::Update()
{
	// Set back ground to be an off white
	Clear(olc::Pixel{ 240, 240, 240, 255 });

	// Lerp fade value from 0.0 to 1.0 back to 0.0
	float fadeVal = SHOW_LOGO_TIME * sin(logoTime);

	// Clamp fade value
	if (fadeVal > 1.0f) fadeVal = 1.0f;
	if (fadeVal < 0.0f) fadeVal = 0.0f;

	int colVal = static_cast<int>(ceil(255.0f * fadeVal));
	olc::Pixel tintColor = olc::Pixel(colVal, colVal, colVal, colVal);

	DrawDecal({ logoPos.x, logoPos.y - 35 }, olc_logo->Decal(), olc_logoSize, tintColor);
	DrawDecal({ logoPos.x, logoPos.y + 35 }, flowe_logo->Decal(), flowe_logoSize, tintColor);

	if (logoTime <= -0.1f)
	{
		currentScene = SCENE::SCENE_MAIN_MENU;
	}

	logoTime -= time->elapsedTime;

	return true;
}