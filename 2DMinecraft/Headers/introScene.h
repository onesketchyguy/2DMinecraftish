#pragma once

#ifndef INTRO_SCENE_H
#define INTRO_SCENE_H

#include "sceneDefinition.h"

class IntroScene : public Scene
{
	olc::Renderable* olc_logo;
	olc::Renderable* flowe_logo;
	olc::vf2d logoPos = { 0.0f, 0.0f };
	olc::vf2d olc_logoSize = { 0.25f, 0.25f };
	olc::vf2d flowe_logoSize = { 1.25f, 1.25f };

	const float TOTAL_LOGO_TIME = 3.0f;
	const float SHOW_LOGO_TIME = 1.5f;

	float logoTime = TOTAL_LOGO_TIME;

public:
	IntroScene() = default;
	~IntroScene()
	{
		delete olc_logo;
		delete flowe_logo;
	}

public:
	bool OnLoad() override;
	bool Update() override;
};

#endif // !INTRO_SCENE_H